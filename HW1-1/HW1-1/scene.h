#ifndef SCENE
#define SCENE

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

class Model {
public:
	string model_name;
	float scale[3];
	float angle;
	float rotate[3];
	float transfer[3];

	Model() {}
};

class Scene {
public:
	vector<Model> model_list;

	Scene(string scene_file);
	virtual ~Scene();

	void LoadScene(string scene_file);
};

#endif