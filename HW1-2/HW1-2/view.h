#ifndef VIEW
#define VIEW

#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

class View {
public:
	float camera_pos[3];
	float camera_lookat[3];
	float up_vector[3];
	float angle;
	float dnear, dfar;
	float view_pos[2];
	float width, height;

	View(string view_file);
	virtual ~View();

	void LoadView(string view_file);
};

#endif