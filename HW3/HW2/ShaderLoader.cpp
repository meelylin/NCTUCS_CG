#include <stdio.h>			//C standard IO
#include <stdlib.h>			//C standard lib
#include <string.h>			//C string lib

#include <glew.h>			//GLEW lib
#include <glut.h>			//GLUT lib

//Function from: http://www.evl.uic.edu/aej/594/code/ogl.cpp
//Read in a textfile (GLSL program)
// we need to pass it as a string to the GLSL driver
char *textFileRead(char *fn)
{
	FILE *fp;
	char *content = NULL;

	int count = 0;

	if (fn != NULL) {

		fp = fopen(fn, "rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);

		}
	}

	return content;
}

//Got this from http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo
// it prints out shader info (debugging!)
void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("printShaderInfoLog: %s\n", infoLog);
		free(infoLog);
	}
	else {
		printf("Shader Info Log: OK\n");
	}
}

//Got this from http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo
// it prints out shader info (debugging!)
void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("printProgramInfoLog: %s\n", infoLog);
		free(infoLog);
	}
	else {
		printf("Program Info Log: OK\n");
	}
}

void setShaders(int& program)
{
	int vertShader, geomShader, fragShader;
	char *vertSource = NULL, *geomSource = NULL, *fragSource = NULL;

	//First, create our shaders 
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	geomShader = glCreateShader(GL_GEOMETRY_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Read in the programs
	vertSource = textFileRead("../HW2/PhongShading.vert");
	geomSource = textFileRead("../HW2/PhongShading.geom");
	fragSource = textFileRead("../HW2/PhongShading.frag");

	//Setup a few constant pointers for below
	const char *vv = vertSource;
	const char *gg = geomSource;
	const char *ff = fragSource;

	glShaderSource(vertShader, 1, &vv, NULL);
	glShaderSource(geomShader, 1, &gg, NULL);
	glShaderSource(fragShader, 1, &ff, NULL);

	free(vertSource);
	free(geomSource);
	free(fragSource);

	glCompileShader(vertShader);
	glCompileShader(geomShader);
	glCompileShader(fragShader);

	program = glCreateProgram();

	glAttachShader(program, vertShader);
	glAttachShader(program, geomShader);
	glAttachShader(program, fragShader);

	glLinkProgram(program);

	//You can use glUseProgram(program) whenever you want to render something with the program.
	//If you want to render with the fixed pipeline, use glUseProgram(0).

	printShaderInfoLog(vertShader);
	printShaderInfoLog(geomShader);
	printShaderInfoLog(fragShader);
	printProgramInfoLog(program);
}
