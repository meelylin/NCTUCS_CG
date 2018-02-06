#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <stdio.h>			//C standard IO
#include <stdlib.h>			//C standard lib
#include <string.h>			//C string lib

#include <glew.h>			//GLEW lib
#include <glut.h>			//GLUT lib

char *textFileRead(char *fn);
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);
void setShaders(int& program);

#endif
