#pragma once

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <gl/freeglut.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

#include <cstdio>

class blockGroup
{
public:
	blockGroup();
	virtual ~blockGroup();

	//generate data inside a blockGroup.
	void Init_sinXsinY(float fx, float fz, float px, float pz, float ax, float az, float groupPosX, float groupPosZ);
	void InitBuffers(GLuint _cs);

	//glUseProgram(...) before call this method
	void GenerateBuffer();

	//glUseProgram(...) before call this method
	//glBindVertexArray(...) before call this method
	//position - normal - tangent - texCoord1&2 / color - instance
	void Draw(int vertCount, int instanceAttribIndex, GLint modelMatrixUniformIndex);
	
	int getPos(int x, int y, int z);

	//blockGroupPos
	glm::vec3 blockGroupPos;

	//the main buffer for a blockGroup.
	unsigned int blockId[32768];
	GLuint instanceCount;
	//GLsizei baseInstanceVertexCount;

	//Buffers used for computation and drawing
	GLuint cs_vao, blockId_ssbo, blockPos_ssbo, instanceCount_ssbo;
	//GLuint baseInstance_cube;
	//GLuint compute_program;

	bool bufferUpdated, bufferInited;
};
