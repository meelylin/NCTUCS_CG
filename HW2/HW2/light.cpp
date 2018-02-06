#include "light.h"
#include <iostream>

Light::Light(string light_file) {
	LoadLight(light_file);
}

Light::~Light() {}

void Light::LoadLight(string light_file) {
	FILE *pfile;
	char type[100];
	float temp;

	pfile = fopen(light_file.c_str(), "r");

	if (!pfile) {
		cout << string("Can not open light File \"") << light_file << "\" !" << endl;
		return;
	}

	while (!feof(pfile)) {

		type[0] = NULL;
		fscanf(pfile, "%s", type);

		if (!strcmp(type, "light")) {
			light temp_light;

			for (int i = 0; i<3; i++) {
				fscanf(pfile, "%f", &temp);
				temp_light.pos[i] = temp;
			}
			temp_light.pos[3] = 1.0f;

			for (int i = 0; i<3; i++) {
				fscanf(pfile, "%f", &temp);
				temp_light.ambient[i] = temp;
			}

			for (int i = 0; i<3; i++) {
				fscanf(pfile, "%f", &temp);
				temp_light.diffuse[i] = temp;
			}

			for (int i = 0; i<3; i++) {
				fscanf(pfile, "%f", &temp);
				temp_light.specular[i] = temp;
			}

			light_list.push_back(temp_light);
		}
		else if (!strcmp(type, "ambient")) {
			for (int i = 0; i<3; i++) {
				fscanf(pfile, "%f", &temp);
				environment_ambient[i] = temp;
			}
		}
		else;
	}
	if (pfile)
		fclose(pfile);
}