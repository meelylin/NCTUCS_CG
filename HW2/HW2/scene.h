#ifndef SCENE
#define SCENE

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

enum texture_type { no, single, multi, cube };

class Model {
public:
	string model_name;
	float scale[3];
	float angle;
	float rotate[3];
	float transfer[3];

	texture_type tex_type;
	string tex_name[6];
	int tex_ID[2];

	Model() {}
};

class Scene {
public:
	vector<Model> model_list;
	int tex_num;

	Scene(string scene_file);
	virtual ~Scene();

	void LoadScene(string scene_file);
};

#endif