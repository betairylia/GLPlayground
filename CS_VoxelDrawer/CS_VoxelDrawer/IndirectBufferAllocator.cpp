#include "IndirectBufferAllocator.h"

IndirectBufferAllocator* IndirectBufferAllocator::Singleton = NULL;

IndirectBufferAllocator::IndirectBufferAllocator()
{
}

IndirectBufferAllocator::~IndirectBufferAllocator()
{
}

void IndirectBufferAllocator::InitIndirectBufferPool()
{
	for (int i = 0; i < INDIRECT_BUFFER_POOL_SIZE; i++)
	{
		indirectCmd[i].baseInstance = 0;
		indirectCmd[i].count = 0;
		indirectCmd[i].first = 0;
		indirectCmd[i].primCount = 1;
	}

	glGenBuffers(1, &indirectBufferObj);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indirectBufferObj);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(indirectCmd), indirectCmd, GL_DYNAMIC_DRAW);
}

int IndirectBufferAllocator::Hash(glm::vec3 chunkPos, int chunkScale)
{
	return chunkScale * ((int)chunkPos.x * 1000 + (int)chunkPos.y * 10 + (int)chunkPos.z) % INDIRECT_BUFFER_POOL_SIZE;
}

int IndirectBufferAllocator::GetNewIndirectBuffer(glm::vec3 chunkPos, int chunkScale)
{
	int index, startPos = Hash(chunkPos, chunkScale);
	index = startPos;

	while (isUsed[index])
	{
		index++;
		index %= INDIRECT_BUFFER_POOL_SIZE;

		if (index == startPos)
		{
			return -1;
		}
	}

	isUsed[index] = true;
	RefreshBufferData(index);

	return index;
}

void IndirectBufferAllocator::RefreshBufferData(int index)
{
	indirectCmd[index].baseInstance = 0;
	indirectCmd[index].count = 0;
	indirectCmd[index].first = 0;
	indirectCmd[index].primCount = 1;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indirectBufferObj);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, index * sizeof(DrawArraysIndirectCommand), sizeof(DrawArraysIndirectCommand), &indirectCmd[index]);
}

bool IndirectBufferAllocator::FreeIndirectBuffer(int index)
{
	if (isUsed[index])
	{
		isUsed[index] = false;
		return true;
	}
	return false;
}

void IndirectBufferAllocator::ReadBackIndirect()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indirectBufferObj);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(indirectCmd), indirectCmd);
}

IndirectBufferAllocator * IndirectBufferAllocator::GetSingleton()
{
	if (Singleton == NULL)
	{
		Singleton = new IndirectBufferAllocator();
		Singleton->InitIndirectBufferPool();
	}

	return Singleton;
}
