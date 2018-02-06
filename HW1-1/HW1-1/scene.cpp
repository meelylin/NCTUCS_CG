#include "scene.h"
#include <iostream>

Scene::Scene(string scene_file) {
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

	while (!feof(pfile)) {

		type[0] = NULL;
		fscanf(pfile, "%s", type);

		if (!strcmp(type, "model")) {
			char obj_name[256];
			Model temp_model;

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
		else;
	}
	if (pfile)
		fclose(pfile);

}