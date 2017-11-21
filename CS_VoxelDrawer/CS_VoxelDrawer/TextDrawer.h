#pragma once

#include <GL/glew.h>
#include <gl/freeglut.h>

#include <gli/gli.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <vector>

#include "ToolBox.h"

class TextDrawer
{
public:
	TextDrawer();
	~TextDrawer();

	void initText2D(const char * texturePath);
	void printText2D(const char * text, int x, int y, int size, int sizeX);
	void cleanupText2D();

	GLuint textTexture, vao_textMesh, vbo_textMeshPos, vbo_textMeshUV;
	GLuint vs, fs, textRenderProgram;
};

