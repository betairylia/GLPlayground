#pragma once

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <gl/freeglut.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

#include <cstdio>

typedef  struct {
	GLuint  count;
	GLuint  primCount;
	GLuint  first;
	GLuint  baseInstance;
} DrawArraysIndirectCommand;

class blockGroup
{
public:
	blockGroup();
	virtual ~blockGroup();

	//generate data inside a blockGroup.
	void Init_sinXsinY(float lambdax, float lambdaz, float px, float pz, float ax, float az, float groupPosX, float groupPosZ);
	void InitBuffers(GLuint _cs);

	//glUseProgram(...) before call this method
	void GenerateBuffer(bool uploadBuffers = false);

	//glUseProgram(...) before call this method
	//glBindVertexArray(...) before call this method
	//position - normal - tangent - texCoord1&2 / color - instance
	void Draw(int vertCount, int instanceAttribIndex, GLint modelMatrixUniformIndex);
	
	int getPos(int x, int y, int z);

	//blockGroupPos
	glm::vec3 blockGroupPos;

	//the main buffer for a blockGroup.
	unsigned int blockId[32768];
	//GLuint instanceCount;
	//GLsizei baseInstanceVertexCount;
	DrawArraysIndirectCommand cmd;

	//Buffers used for computation and drawing
	GLuint cs_vao, blockId_ssbo, blockPos_ssbo, indirectBuffer_ssbo, indirectBuffer_ssbo_cpy;
	//GLuint baseInstance_cube;
	//GLuint compute_program;

	bool bufferUpdated, bufferInited;
};

