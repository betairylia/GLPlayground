#pragma once

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <gl/freeglut.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

#include <cstdio>
#include <iostream>

#include "blockGroup.h"
#include "VariablePool.h"

class ChunkOctreeNode
{
public:
	ChunkOctreeNode() 
	{
		memset(child, 0, sizeof(child));
		group = NULL;
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
	void ClearGroup();
	void CleanChildResc();

	ChunkOctreeNode* GetMostLeft();
	ChunkOctreeNode* GetMostRight();

	//For multi thread updating
	bool needExpand, hadChild;
};

