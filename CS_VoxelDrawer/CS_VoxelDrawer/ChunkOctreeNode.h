#pragma once

#include <GL/glew.h>
#include <gl/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>
#include <iostream>
#include <vector>

#include "blockGroup.h"
#include "VariablePool.h"

class ChunkOctreeNode
{
public:
	typedef struct
	{
		bool isBuild;
		bool needDelete;
		ChunkOctreeNode* node;
		blockGroup* groupBak;
	}GPUWork;

	ChunkOctreeNode() 
	{
		memset(child, 0, sizeof(child));
		group = NULL;

		isReady = false;
		childVisible = true;
		inLinkedList = false;
	}
	ChunkOctreeNode(glm::vec3 _pos, glm::vec3 _centerPos, int _scale, ChunkOctreeNode *fa, int index);
	virtual ~ChunkOctreeNode();

	glm::vec3 pos, centerPos;
	int scale;
	
	ChunkOctreeNode *child[8], *next, *prev, *father;
	int fatherChildIndex;
	
	blockGroup *group;

	//Temporarily remove a child
	//bool childMask[8];

	void CreateGroup();
	void BuildGroupData();
	void InitGroupMesh();
	void BuildGroupMesh();
	void FreeGroupBuffer();
	void ClearGroup();

	void InList(std::vector<GPUWork>& list, bool isBuild, bool needDelete);
	void OutList(std::vector<GPUWork>& list, bool isBuild, bool isDelete = false);

	bool hasChild();

	void SelfInLinkedList();
	void _SetSubTreeInLinkedList(ChunkOctreeNode * node, bool isIn);
	void SubTreeInLinkedList();

	ChunkOctreeNode* GetMostLeft();
	ChunkOctreeNode* GetMostRight();

	//For multi thread updating
	bool needExpand, hadChild, isReady, isReadyPrev, childVisible, inLinkedList;//, groupReady;
	bool inListBuild, inListDestroy;
};

