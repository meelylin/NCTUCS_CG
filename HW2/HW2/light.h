#ifndef LIGHT
#define LIGHT

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

class light {
public:
	float pos[4];
	float ambient[3];
	float diffuse[3];
	float specular[3];

	light() {}
};

class Light {
public:
	float environment_ambient[3];
	vector<light> light_list;

	Light(string light_file);
	virtual ~Light();

	void LoadLight(string light_file);
};

#endif