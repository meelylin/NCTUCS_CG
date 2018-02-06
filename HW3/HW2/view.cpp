#include "view.h"
#include <iostream>

View::View(string view_file) {
	LoadView(view_file);
}

View::~View() {}

void View::LoadView(string view_file) {
	FILE *pfile;
	char type[100];
	float temp[4];

	pfile = fopen(view_file.c_str(), "r");

	if (!pfile) {
		cout << string("Can not open view File \"") << view_file << "\" !" << endl;
		return;
	}

	while (!feof(pfile)) {

		type[0] = NULL;
		fscanf(pfile, "%s", type);

		if (!strcmp(type, "eye")) {
			fscanf(pfile, "%f %f %f", &temp[0], &temp[1], &temp[2]);
			camera_pos[0] = temp[0];
			camera_pos[1] = temp[1];
			camera_pos[2] = temp[2];
		}
		else if (!strcmp(type, "vat")) {
			fscanf(pfile, "%f %f %f", &temp[0], &temp[1], &temp[2]);
			camera_lookat[0] = temp[0];
			camera_lookat[1] = temp[1];
			camera_lookat[2] = temp[2];
		}
		else if (!strcmp(type, "vup")) {
			fscanf(pfile, "%f %f %f", &temp[0], &temp[1], &temp[2]);
			up_vector[0] = temp[0];
			up_vector[1] = temp[1];
			up_vector[2] = temp[2];
		}
		else if (!strcmp(type, "fovy")) {
			fscanf(pfile, "%f", &temp[0]);
			angle = temp[0];
		}
		else if (!strcmp(type, "dnear")) {
			fscanf(pfile, "%f", &temp[0]);
			dnear = temp[0];
		}
		else if (!strcmp(type, "dfar")) {
			fscanf(pfile, "%f", &temp[0]);
			dfar = temp[0];
		}
		else if (!strcmp(type, "viewport")) {
			fscanf(pfile, "%f %f %f %f", &temp[0], &temp[1], &temp[2], &temp[3]);
			view_pos[0] = temp[0];
			view_pos[1] = temp[1];
			width = temp[2];
			height = temp[3];
		}
		else;
	}
	if (pfile)
		fclose(pfile);

}