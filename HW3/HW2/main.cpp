/* 0416209_Assignment3 */
/*
UI control:
Keyboard : Move the camera.
'w' : zoom in
'a' : move left (circle the center)
's' : zoom out
'd' : move right (circle the center)
'f' : move light position(Decrease y)
'r' : move light position(Increase y)
*/
#include "FreeImage.h"
#include "glew.h"
#include "mesh.h"
#include "glut.h"
#include "view.h"
#include "scene.h"
#include "light.h"
#include "ShaderLoader.h"
#include "glm\glm.hpp"

#define PI 3.14159265

int program;

mesh **object;
View *view;
Scene *scene;
Light *light;

string scene_dir = "as3";
string view_file = scene_dir + "/" + scene_dir + ".view";
string scene_file = scene_dir + "/" + scene_dir + ".scene";
string light_file = scene_dir + "/" + scene_dir + ".light";

// UI control : zoom & rotate 
const int zoom_delta = 10;
const float rotate_delta = 10;
double camera_radius = 0;
double camera_rotation = 0;

int windowSize[2];
int select = -1;	// UI conyrol : keyboard select object
float x_old, y_old, x_vec, y_vec;
int level = 0;

unsigned *texObject;

GLhandleARB PhongShading;

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
void LoadTexture(const char* pFilename, int texID, int tex_target, int tex_option);

int main(int argc, char** argv)
{
	light = new Light(light_file);

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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Assignment3");

	glewInit();
	FreeImage_Initialise();
	glGenTextures(scene->tex_num + 1, texObject);

	for (int i = 0; i < (int)scene->model_list.size(); i++) {
		switch (scene->model_list[i].tex_type) {
		case single:
			LoadTexture((scene_dir + "/" + scene->model_list[i].tex_name[0]).c_str(), scene->model_list[i].tex_ID[0], GL_TEXTURE_2D, GL_TEXTURE_2D);
			break;
		case multi:
			LoadTexture((scene_dir + "/" + scene->model_list[i].tex_name[0]).c_str(), scene->model_list[i].tex_ID[0], GL_TEXTURE_2D, GL_TEXTURE_2D);
			LoadTexture((scene_dir + "/" + scene->model_list[i].tex_name[1]).c_str(), scene->model_list[i].tex_ID[1], GL_TEXTURE_2D, GL_TEXTURE_2D);
			break;
		case cube:
			LoadTexture((scene_dir + "/" + scene->model_list[i].tex_name[0]).c_str(), scene->model_list[i].tex_ID[0], GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
			LoadTexture((scene_dir + "/" + scene->model_list[i].tex_name[1]).c_str(), scene->model_list[i].tex_ID[0], GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
			LoadTexture((scene_dir + "/" + scene->model_list[i].tex_name[2]).c_str(), scene->model_list[i].tex_ID[0], GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
			LoadTexture((scene_dir + "/" + scene->model_list[i].tex_name[3]).c_str(), scene->model_list[i].tex_ID[0], GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
			LoadTexture((scene_dir + "/" + scene->model_list[i].tex_name[4]).c_str(), scene->model_list[i].tex_ID[0], GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
			LoadTexture((scene_dir + "/" + scene->model_list[i].tex_name[5]).c_str(), scene->model_list[i].tex_ID[0], GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
			break;
		}
	}

	FreeImage_DeInitialise();
	setShaders(program);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMainLoop();

	return 0;
}

void LoadTexture(const char* pFilename, int texID, int tex_target, int tex_option) {
	FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(pFilename, 0), pFilename);
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);

	glBindTexture(tex_target, texObject[texID]);
	glTexParameteri(tex_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(tex_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(tex_option, 0, GL_RGBA8, iWidth, iHeight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	glGenerateMipmap(tex_target);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);
}

void lighting()
{
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

																	 //注意light位置的設定，要在gluLookAt之後
	lighting();

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "lightNumber"), light->light_list.size());
	glUniform1i(glGetUniformLocation(program, "colorTexture"), 0);
	glUniform1i(glGetUniformLocation(program, "level"), level);

	show();
	glUseProgram(0);
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

		// Enable & Bind
		switch (scene->model_list[k].tex_type) {
		case single:
			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texObject[scene->model_list[k].tex_ID[0]]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			break;
		case multi:
			for (int a = 0; a<2; a++) {
				glActiveTexture(GL_TEXTURE0 + a);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texObject[scene->model_list[k].tex_ID[a]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			}
			break;
		case cube:
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_R);
			glEnable(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[scene->model_list[k].tex_ID[0]]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			break;
		}

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
				switch (scene->model_list[k].tex_type) {
				case single:
					glTexCoord2fv(current->tList[current->faceList[i][j].t].ptr);
					break;
				case multi:
					glMultiTexCoord2fv(GL_TEXTURE0, current->tList[current->faceList[i][j].t].ptr);
					glMultiTexCoord2fv(GL_TEXTURE1, current->tList[current->faceList[i][j].t].ptr);
					break;
				}
				glNormal3fv(current->nList[current->faceList[i][j].n].ptr);
				glVertex3fv(current->vList[current->faceList[i][j].v].ptr);
			}
			glEnd();
		}

		// Disable & Unbind
		switch (scene->model_list[k].tex_type) {
		case single:
			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			break;
		case multi:
			for (int a = 0; a<2; a++) {
				glActiveTexture(GL_TEXTURE0 + a);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			break;
		case cube:
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			break;
		}

		glPopMatrix();
	}
}

void viewing() {
	view = new View(view_file);
}

void scene_model() {
	scene = new Scene(scene_file);
	texObject = new unsigned[scene->tex_num + 1];
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
	case 'f': 		// move light position(Decrease y)
		(light->light_list[0]).pos[1] -= 1.0f;
		break;
	case 'r':		// move light position(Increase y)
		(light->light_list[0]).pos[1] += 1.0f;
		break;
	default:		// select object
		key -= '0';
		if (key >= 0 && key <= 9) {
			level = key;
		}
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
