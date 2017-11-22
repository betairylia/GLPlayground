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
		groupReady = false;
	}
	ChunkOctreeNode(glm::vec3 _pos, glm::vec3 _centerPos, int _scale);
	virtual ~ChunkOctreeNode();

	glm::vec3 pos, centerPos;
	int scale;
	ChunkOctreeNode* child[8], *next, *prev;
	blockGroup* group;

	void CreateGroup();
	void BuildGroupData();
	void InitGroupMesh();
	void BuildGroupMesh();
	void FreeGroupBuffer();
	void ClearGroup();

	void InList(std::vector<GPUWork>& list, bool isBuild, bool needDelete);
	void OutList(std::vector<GPUWork>& list, bool isBuild);

	bool hasChild();

	ChunkOctreeNode* GetMostLeft();
	ChunkOctreeNode* GetMostRight();

	//For multi thread updating
	bool needExpand, hadChild, isReady, groupReady;
	bool inListBuild, inListDestroy;
};

