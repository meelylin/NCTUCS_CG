/* 0416209_Assignment2 */
/*
UI control:
Keyboard : Move the camera.
'w' : zoom in
'a' : move left (circle the center)
's' : zoom out
'd' : move right (circle the center)
DOF target select :
'1' : focus on dice (-10, 12, 0)
'2' : focus on 1st reflection (-70, 12, 0)
'3' : focus on 2nd reflection (-170, 12, 0)
*/
#include "FreeImage.h"
#include "glew.h"
#include "mesh.h"
#include "glut.h"
#include "view.h"
#include "scene.h"
#include "light.h"
#include "glm\glm.hpp"

#define PI 3.14159265

using namespace glm;

mesh **object;
View *view;
Scene *scene;
Light *light;

string scene_dir = "CornellBox";
string view_file = scene_dir + "/" + scene_dir + ".view";
string scene_file = scene_dir + "/" + scene_dir + ".scene";
string light_file = scene_dir + "/" + scene_dir + ".light";

// UI control : zoom & rotate 
const int zoom_delta = 10;
const float rotate_delta = 10;
double camera_radius = 0;
double camera_rotation = 0;

float aperture = 0.2;	// for DOF jitter intensity
vec3 target;			// DOF : selected target

int windowSize[2];
int select = -1;		// UI conyrol : keyboard select object
float x_old, y_old, x_vec, y_vec;
int DOF = 0;

unsigned *texObject;

void lighting();
void display();
void draw_reflect(int k, float d, int reverse);
void draw(int m);
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

	// initialize camera radius : circle the object (x-z plane)
	camera_radius = sqrt(pow(view->camera_pos[0] - view->camera_lookat[0], 2) + pow(view->camera_pos[2] - view->camera_lookat[2], 2));
	// initialize camera rotation angle : circle the object ((x-z plane)
	camera_rotation = atan((view->camera_pos[2] - view->camera_lookat[2]) / (view->camera_pos[0] - view->camera_lookat[0]));

	glutInit(&argc, argv);
	glutInitWindowSize(view->width, view->height);
	glutInitWindowPosition(view->view_pos[0], view->view_pos[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL | GLUT_ACCUM);
	glutCreateWindow("Assignment2");
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
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);
	glEnable(GL_CULL_FACE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

																									 // viewport transformation
	glViewport(view->view_pos[0], view->view_pos[1], windowSize[0], windowSize[1]);

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

	if (DOF == 1)
		target = vec3(-10.0, 12.0, 0.0);
	else if (DOF == 2)
		target = vec3(-70.0, 12.0, 0.0);
	else if (DOF == 3)
		target = vec3(-170.0, 12.0, 0.0);

	//~~~~~~ add deapth of field condition ~~~~~~~
	if (DOF == 1 || DOF == 2 || DOF == 3)
	{
		vec3 vec3_up = vec3(view->up_vector[0], view->up_vector[1], view->up_vector[2]);
		vec3 vec3_object = vec3(view->camera_lookat[0], view->camera_lookat[1], view->camera_lookat[2]);
		vec3 vec3_eye = vec3(view->camera_pos[0], view->camera_pos[1], view->camera_pos[2]);
		vec3 vec3_right = normalize(cross(vec3_object - vec3_eye, vec3_up));

		for (int i = 0; i<8; i++)
		{
			vec3 jitter = vec3_right * cosf(i * 2 * PI / 8) + vec3_up * sinf(i * 2 * PI / 8);
			vec3 jitter_eye = vec3_eye + jitter * aperture;

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(jitter_eye.x, jitter_eye.y, jitter_eye.z, // eye
				target.x, target.y, target.z, // center
				vec3_up.x, vec3_up.y, vec3_up.z); // up

			lighting();

			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDepthMask(GL_FALSE);
			glCullFace(GL_BACK);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			// set stencil buffer value, draw mirror
			draw(10);
			draw(11);

			glFrontFace(GL_CCW);		// select counterclockwise polygons as front-facing (default)
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);	// only change stencil buffer, don't change frame buffer	
			glDepthMask(GL_TRUE);
			glStencilFunc(GL_EQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);		// decrease stencil buffer where objects cover (in front of) the mirror
														// set stencil buffer value
			draw(8);
			draw(9);
			for (int i = 0; i<8; i++)
				draw(i);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);	// also change frame buffer
			glDepthMask(GL_TRUE);
			glStencilFunc(GL_EQUAL, 0, 0xFF);		// only deal with stencil buffer = 0 (not in the mirror)
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			// draw scene not in the mirror
			draw(8);
			draw(9);
			for (int i = 0; i<8; i++)
				draw(i);

			glStencilFunc(GL_EQUAL, 1, 0xFF);		// only deal with stencil buffer = 1 (in the mirror, reflection)	
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			// draw reflection
			for (int pass = 1; pass < 5; pass++)
			{
				if (pass % 2 == 1)
				{
					glFrontFace(GL_CW);	// select clockwise polygons as front-facing

										// draw reflection of front mirror
					draw_reflect(8, 80 * pass, 1);
					draw_reflect(9, 80 * pass, 1);
					for (int i = 0; i<8; i++)
						draw_reflect(i, 80 * pass, 1);

					// draw reflection of back mirror
					draw_reflect(8, -80 * pass, 1);
					draw_reflect(9, -80 * pass, 1);
					for (int i = 0; i<8; i++)
						draw_reflect(i, -80 * pass, 1);
				}
				else
				{
					glFrontFace(GL_CCW);	// select counterclockwise polygons as front-facing (default)

											// draw reflection of front mirror
					draw_reflect(8, -80 * pass, 0);
					draw_reflect(9, -80 * pass, 0);
					for (int i = 0; i<8; i++)
						draw_reflect(i, -80 * pass, 0);

					// draw reflection of back mirror
					draw_reflect(8, 80 * pass, 0);
					draw_reflect(9, 80 * pass, 0);
					for (int i = 0; i<8; i++)
						draw_reflect(i, 80 * pass, 0);
				}
			}
			glClearDepth(1.0f);
			glClearStencil(0);
			glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glAccum(i ? GL_ACCUM : GL_LOAD, 1.0f / 8);
		}
	}
	else
	{
		lighting();

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glCullFace(GL_BACK);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		// draw mirror
		draw(10);
		draw(11);

		glFrontFace(GL_CCW);	// select counterclockwise polygons as front-facing (default)	
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);	// only change stencil buffer, don't change frame buffer	
		glDepthMask(GL_FALSE);
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);		// decrease the stencil buffer where objects cover (in front of) the mirror
													// set stencil buffer value
		draw(8);
		draw(9);
		for (int i = 0; i<8; i++)
			draw(i);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);	// also change frame buffer
		glDepthMask(GL_TRUE);
		glStencilFunc(GL_EQUAL, 0, 0xFF);		// only deal with stencil buffer = 0 (not in the mirror)
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		// draw scene not in the mirror
		draw(8);
		draw(9);
		for (int i = 0; i<8; i++)
			draw(i);

		glStencilFunc(GL_EQUAL, 1, 0xFF);		// only deal with stencil buffer = 1 (in the mirror, reflection)	
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		// draw reflection
		for (int pass = 1; pass < 5; pass++)
		{
			if (pass % 2 == 1)
			{
				glFrontFace(GL_CW);		// select clockwise polygons as front-facing

										// draw reflection of front mirror
				draw_reflect(8, 80 * pass, 1);
				draw_reflect(9, 80 * pass, 1);
				for (int i = 0; i<8; i++)
					draw_reflect(i, 80 * pass, 1);

				// draw reflection of back mirror
				draw_reflect(8, -80 * pass, 1);
				draw_reflect(9, -80 * pass, 1);
				for (int i = 0; i<8; i++)
					draw_reflect(i, -80 * pass, 1);
			}
			else
			{
				glFrontFace(GL_CCW);	// select counterclockwise polygons as front-facing (default)

										// draw reflection of front mirror
				draw_reflect(8, -80 * pass, 0);
				draw_reflect(9, -80 * pass, 0);
				for (int i = 0; i<8; i++)
					draw_reflect(i, -80 * pass, 0);

				// draw reflection of back mirror
				draw_reflect(8, 80 * pass, 0);
				draw_reflect(9, 80 * pass, 0);
				for (int i = 0; i<8; i++)
					draw_reflect(i, 80 * pass, 0);
			}

		}
		glAccum(GL_ACCUM, 1.0f);
	}

	glAccum(GL_RETURN, 1.0);

	glFrontFace(GL_CCW);

	//注意light位置的設定，要在gluLookAt之後
	//lighting();
	//show();
	glutSwapBuffers();
}

void draw_reflect(int k, float d, int reverse) {
	mesh *current;
	int lastMaterial;

	current = object[k];
	lastMaterial = -1;

	glPushMatrix();

	if (reverse)
		glScalef(-1.0, 1.0, 1.0);

	glTranslatef(scene->model_list[k].transfer[0] + d, scene->model_list[k].transfer[1], scene->model_list[k].transfer[2]);
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
			glNormal3fv(current->nList[current->faceList[i][j].n].ptr);
			glVertex3fv(current->vList[current->faceList[i][j].v].ptr);
		}
		glEnd();
	}
	glPopMatrix();
}

void draw(int k) {
	mesh *current;
	int lastMaterial;

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
			glNormal3fv(current->nList[current->faceList[i][j].n].ptr);
			glVertex3fv(current->vList[current->faceList[i][j].v].ptr);
		}
		glEnd();
	}
	glPopMatrix();
}

void viewing() {
	view = new View(view_file);
}

void scene_model() {
	scene = new Scene(scene_file);
}

void meshing() {
	object = new mesh*[(int)scene->model_list.size()];

	for (int i = 0; i < (int)scene->model_list.size(); i++) {
		string mesh_name = scene_dir + "/" + scene->model_list[i].model_name;
		object[i] = new mesh(mesh_name.c_str());
	}
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w': 		// zoom in
		camera_radius -= zoom_delta;
		break;
	case 's':		//zoom out
		camera_radius += zoom_delta;
		break;
	case 'a':		// move left (circle the center object)
		camera_rotation += rotate_delta / 180 * PI;		// 1 PI = 180 degree
		break;
	case 'd':		// move right (circle the center object)
		camera_rotation -= rotate_delta / 180 * PI; 	// 1 PI = 180 degree
		break;
	default:		// select object
		key -= '0';
		if (key >= 0 && key <= 3)
			DOF = key;
	}
	//Set new position of camera
	//x = r*sin(alpha), z = r*cos(alpha)
	view->camera_pos[0] = view->camera_lookat[0] + camera_radius*cos(camera_rotation);
	view->camera_pos[2] = view->camera_lookat[2] + camera_radius*sin(camera_rotation);

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
