#pragma once

#include "ChunkOctreeNode.h"

#include <vector>
#include <algorithm>

const int mapBigChunkLenth = 4;

//TODO: Update blockGroups

class ChunkOctree
{
public:
	ChunkOctree();
	virtual ~ChunkOctree();

	void Update(glm::vec3 playerPos);
	void UpdateNode(ChunkOctreeNode* node);

	void Drawall(int vertCount, int instanceAttribIndex, GLint modelMatrixUniformIndex);

	ChunkOctreeNode* mp_treeRoot[mapBigChunkLenth][mapBigChunkLenth];
	ChunkOctreeNode renderList[mapBigChunkLenth][mapBigChunkLenth];

	glm::vec3 m_playerPos;
};

