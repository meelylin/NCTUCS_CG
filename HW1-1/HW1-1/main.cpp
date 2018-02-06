/* 0416209_Assignment1-1 */
/*
UI control:
Keyboard : Move the camera.
'w' : zoom in
'a' : move left (circle the center)
's' : zoom out
'd' : move right (circle the center)
'1'~'9' : select n-th object
Mouse : Drag the selected object.
Right : Increase x
Left : Decrease x
Up : Increase y
Down : Decrease y
*/
#include "mesh.h"
#include "glut.h"
#include "view.h"
#include "scene.h"
#include "light.h"

mesh **object;
View *view;
Scene *scene;
Light *light;

string view_file = "view.view";
string scene_file = "scene.scene";
string light_file = "light.light";

int windowSize[2];
int select = -1;	// UI conyrol : keyboard select object
float zoom = 0.0f, rotate_angle = 0.0f;
float x_old, y_old, x_vec, y_vec;

void lighting();
void display();
void show();
void reshape(GLsizei, GLsizei);
void viewing();
void scene_model();
void meshing();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

int main(int argc, char** argv)
{
	viewing();
	scene_model();
	meshing();

	glutInit(&argc, argv);
	glutInitWindowSize(view->width, view->height);
	glutInitWindowPosition(view->view_pos[0], view->view_pos[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Assignment1-1");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMainLoop();

	return 0;
}

void lighting()
{
	light = new Light(light_file);

	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);

	// set light property
	for (int i = 0; i < (int)light->light_list.size(); i++) {
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, light->light_list[i].pos);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light->light_list[i].diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light->light_list[i].specular);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light->light_list[i].ambient);
	}
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light->environment_ambient);
}

void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(view->angle, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], view->dnear, view->dfar);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(view->camera_pos[0], view->camera_pos[1], view->camera_pos[2], // eye
			  view->camera_lookat[0], view->camera_lookat[1], view->camera_lookat[2], // center
			  view->up_vector[0], view->up_vector[1], view->up_vector[2]); // up

	// UI control : keyboard zoom in/out
	glTranslatef(view->camera_pos[0] * zoom, view->camera_pos[1] * zoom, view->camera_pos[2] * zoom);
	// UI control : keyboard move left/right. rotation follows the right-hand rule
	glRotatef(rotate_angle, view->camera_lookat[0] + view->up_vector[0]
						  , view->camera_lookat[1] + view->up_vector[1]
						  , view->camera_lookat[2] + view->up_vector[2]);

	//注意light位置的設定，要在gluLookAt之後
	lighting();
	show();
	glutSwapBuffers();
}

void show() {
	mesh *current;
	int lastMaterial;

	for (int k = 0; k < (int)scene->model_list.size(); k++) {
		current = object[k];
		lastMaterial = -1;

		glPushMatrix();
		glTranslatef(scene->model_list[k].transfer[0], scene->model_list[k].transfer[1], scene->model_list[k].transfer[2]);
		glRotatef(scene->model_list[k].angle, scene->model_list[k].rotate[0], scene->model_list[k].rotate[1], scene->model_list[k].rotate[2]);
		glScalef(scene->model_list[k].scale[0], scene->model_list[k].scale[1], scene->model_list[k].scale[2]);

		for (size_t i = 0;i < current->fTotal;++i)
		{
			// set material property if this face used different material
			if (lastMaterial != current->faceList[i].m)
			{
				lastMaterial = (int)current->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT, current->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, current->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, current->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &current->mList[lastMaterial].Ns);

				//you can obtain the texture name by object->mList[lastMaterial].map_Kd
				//load them once in the main function before mainloop
				//bind them in display function here
			}

			glBegin(GL_TRIANGLES);
			for (size_t j = 0;j<3;++j)
			{
				//textex corrd. object->tList[object->faceList[i][j].t].ptr
				glNormal3fv(current->nList[current->faceList[i][j].n].ptr);
				glVertex3fv(current->vList[current->faceList[i][j].v].ptr);
			}
			glEnd();
		}

		glPopMatrix();
	}
}

void viewing() {
	view = new View(view_file);
}

void scene_model() {
	scene = new Scene(scene_file);
}

void meshing() {
	object = new mesh*[(int)scene->model_list.size()];

	for (int i = 0; i < (int)scene->model_list.size(); i++)
		object[i] = new mesh(scene->model_list[i].model_name.c_str());
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {
	switch(key){
		case 'w':		// zoom in
			zoom += 0.01f;
			break;
		case 's':		// zoom out
			zoom -= 0.01f;
			break;
		case 'a':		// camera move left
			rotate_angle += 10.0f;
			break;
		case 'd':		// camera move right
			rotate_angle -= 10.0f;
			break;
		default:		// select object
			key -= '0';
			if (key >= 1 && key <= 9) {
				if (key <= (int)scene->model_list.size()) {
					select = key - 1;
					x_old = scene->model_list[select].transfer[0];
					y_old = scene->model_list[select].transfer[1];
				}
			}
	}	
	
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		if (select >= 0) {
			x_vec = x - x_old;
			y_vec = y + y_old;
		}
	}
	else if (state == GLUT_UP) {
		if (select >= 0) {
			x_old = scene->model_list[select].transfer[0];
			y_old = scene->model_list[select].transfer[1];
		}
	}
}

void motion(int x, int y) {
	if (select >= 0) {	// dragging selected object
		scene->model_list[select].transfer[0] = x - x_vec;
		scene->model_list[select].transfer[1] = y_vec - y;
		glutPostRedisplay();
	}
}
