#include "scene.h"
#include <iostream>

Scene::Scene(string scene_file) {
	tex_num = 0;
	LoadScene(scene_file);
}

Scene::~Scene() {}

void Scene::LoadScene(string scene_file) {
	FILE *pfile;
	char type[100];
	float temp;

	pfile = fopen(scene_file.c_str(), "r");

	if (!pfile) {
		cout << string("Can not open scene File \"") << scene_file << "\" !" << endl;
		return;
	}

	Model temp_model;
	while (!feof(pfile)) {

		type[0] = NULL;
		fscanf(pfile, "%s", type);

		if (!strcmp(type, "model")) {
			char obj_name[256];
			fscanf(pfile, "%s", obj_name);
			string temp_name(obj_name);
			temp_model.model_name = temp_name;

			for (int i = 0; i<3; i++) {
				fscanf(pfile, "%f", &temp);
				temp_model.scale[i] = temp;
			}

			fscanf(pfile, "%f", &temp);
			temp_model.angle = temp;

			for (int i = 0; i<3; i++) {
				fscanf(pfile, "%f", &temp);
				temp_model.rotate[i] = temp;
			}

			for (int i = 0; i<3; i++) {
				fscanf(pfile, "%f", &temp);
				temp_model.transfer[i] = temp;
			}

			model_list.push_back(temp_model);
		}
		else if (!strcmp(type, "single-texture")) {
			temp_model.tex_type = single;

			char Tex_name[256];
			fscanf(pfile, "%s", Tex_name);
			temp_model.tex_name[0] = Tex_name;

			tex_num++;
			temp_model.tex_ID[0] = tex_num;
		}
		else if (!strcmp(type, "multi-texture")) {
			temp_model.tex_type = multi;

			for (int i = 0; i<2; i++) {
				char Tex_name[256];
				fscanf(pfile, "%s", Tex_name);
				temp_model.tex_name[i] = Tex_name;

				tex_num++;
				temp_model.tex_ID[i] = tex_num;
			}
		}
		else if (!strcmp(type, "cube-map")) {
			temp_model.tex_type = cube;

			for (int i = 0; i<6; i++) {
				char Tex_name[256];
				fscanf(pfile, "%s", Tex_name);
				temp_model.tex_name[i] = Tex_name;
			}

			tex_num++;
			temp_model.tex_ID[0] = tex_num;
		}
		else if (!strcmp(type, "no-texture")) {
			temp_model.tex_type = no;
		}
		else;
	}
	if (pfile)
		fclose(pfile);

}