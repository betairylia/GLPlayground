#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <gl/freeglut.h>

#include <glm/glm.hpp>
#include <gli/gli.hpp>

#include <iostream>

#include "VariablePool.h"
#include "CImg.h"

using namespace cimg_library;

class ToolBox
{
public:
	ToolBox();
	~ToolBox();

	static GLuint CreateTexture(char const* Filename);
	static char* filetobuf(char *file);

	static void printError();
	static void CheckProgramLinking(char* programName, GLuint program);
	static void CheckShaderCompiling(char* shaderName, GLuint shader);

	static void LoadMap(char* height, char* color);
};

