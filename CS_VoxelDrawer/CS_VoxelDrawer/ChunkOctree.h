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

	//isBuild:		should this chunk be builded. True = build chunk, False = destruct chunk.
	//needDelete:	should this chunkNode be freed after destruction.
	//node:			pointer to chunkNode.
	//groupBak:		pointer to the blockGroup attached to the chunkNode.
	//				N.B. node->group will be set to NULL as soon as the node was added into the destruct list,
	//					 so we need save the pointer to the node's blockGroup inorder to clean it up.
	//					 If we do not set node->group to NULL as soon as the node was added into the destruct list,
	//					 the blockGroup of that node will keep cleaned up and reused while the node comes a leaf node again,
	//					 without rebuild the group buffers.

	ChunkOctree(std::mutex& _m, std::condition_variable& _cv, bool useMT);
	virtual ~ChunkOctree();

	void Update(glm::vec3 playerPos);

	//Single thread updating
	void UpdateNode(ChunkOctreeNode* node);
	
	//Multi thread updating
	void PreUpdateNode(ChunkOctreeNode* node);
	void DoWork();
	bool PostUpdateNode(ChunkOctreeNode* node);
	void CleanChildResc(ChunkOctreeNode* node);

	std::vector<ChunkOctreeNode::GPUWork> workList;
	std::vector<ChunkOctreeNode::GPUWork> GPUworkList;
	std::mutex& m_mutex;
	std::condition_variable& m_condVar;

	void Drawall(int vertCount, int instanceAttribIndex, GLint modelMatrixUniformIndex);

	ChunkOctreeNode* mp_treeRoot[mapBigChunkLenth][mapBigChunkLenth];
	ChunkOctreeNode renderList[mapBigChunkLenth][mapBigChunkLenth];

	glm::vec3 m_playerPos;

	GLuint compute_programme;

	bool multiThread;
};

