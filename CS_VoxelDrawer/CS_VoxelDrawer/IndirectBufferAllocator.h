#pragma once

#include <GL/glew.h>
#include <gl/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef  struct {
	GLuint  count;
	GLuint  primCount;
	GLuint  first;
	GLuint  baseInstance;
} DrawArraysIndirectCommand;

#define INDIRECT_BUFFER_POOL_SIZE 16384

class IndirectBufferAllocator
{
public:
	IndirectBufferAllocator();
	~IndirectBufferAllocator();

	GLuint indirectBufferObj;

	void InitIndirectBufferPool();

	int Hash(glm::vec3 chunkPos, int chunkScale);
	int GetNewIndirectBuffer(glm::vec3 chunkPos, int chunkScale);
	void RefreshBufferData(int index);
	bool FreeIndirectBuffer(int index);

	void ReadBackIndirect();

	static IndirectBufferAllocator* GetSingleton();

private:
	static IndirectBufferAllocator* Singleton;

	DrawArraysIndirectCommand indirectCmd[INDIRECT_BUFFER_POOL_SIZE];
	bool isUsed[INDIRECT_BUFFER_POOL_SIZE];
};

