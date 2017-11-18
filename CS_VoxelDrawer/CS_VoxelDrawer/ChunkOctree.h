#pragma once

#include "ChunkOctreeNode.h"

#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>

const int mapBigChunkLenth = 1;

//TODO: Update blockGroups

class ChunkOctree
{
public:
	ChunkOctree(std::mutex& _m, std::condition_variable& _cv, bool useMT);
	virtual ~ChunkOctree();

	void Update(glm::vec3 playerPos);

	//Single thread updating
	void UpdateNode(ChunkOctreeNode* node);
	
	//Multi thread updating
	void PreUpdateNode(ChunkOctreeNode* node);
	void DoWork();
	void PostUpdateNode(ChunkOctreeNode* node);

	std::vector<ChunkOctreeNode*> workList;
	std::vector<ChunkOctreeNode*> GPUworkList;
	std::mutex& m_mutex;
	std::condition_variable& m_condVar;

	void Drawall(int vertCount, int instanceAttribIndex, GLint modelMatrixUniformIndex);

	ChunkOctreeNode* mp_treeRoot[mapBigChunkLenth][mapBigChunkLenth];
	ChunkOctreeNode renderList[mapBigChunkLenth][mapBigChunkLenth];

	glm::vec3 m_playerPos;

	GLuint compute_programme;

	bool multiThread;
};

