#include "ChunkOctreeNode.h"

ChunkOctreeNode::ChunkOctreeNode(glm::vec3 _pos, glm::vec3 _centerPos, int _scale, ChunkOctreeNode *fa, int index) :
	pos(_pos), 
	centerPos(_centerPos), 
	scale(_scale),
	father(fa),
	fatherChildIndex(index)
{
	memset(child, 0, sizeof(child));
	group = NULL;

	isReady = false;
	childVisible = true;
}

ChunkOctreeNode::~ChunkOctreeNode()
{
	if (father != NULL && father->child[fatherChildIndex] == this)
	{
		father->child[fatherChildIndex] = NULL;
	}
}

void ChunkOctreeNode::CreateGroup()
{
	//TODO
	group = new blockGroup(true, (float)scale);
}

void ChunkOctreeNode::BuildGroupData()
{
	//TODO
	group->Init_sinXsinY(
		VariablePool::lambdax, VariablePool::lambdaz,
		VariablePool::px, VariablePool::pz,
		VariablePool::ax, VariablePool::az,
		pos.x, pos.z, pos.y);
}

void ChunkOctreeNode::InitGroupMesh()
{
	group->InitBuffers();
}

void ChunkOctreeNode::BuildGroupMesh()
{
	group->GenerateBuffer(false, VariablePool::cs_ChunkMeshGeneration_ScaleIndex);
}

void ChunkOctreeNode::FreeGroupBuffer()
{
	if (group != NULL)
	{
		group->FreeBuffers();
	}
}

void ChunkOctreeNode::ClearGroup()
{
	//TODO
	delete group;
	group = NULL;
}

void ChunkOctreeNode::InList(std::vector<GPUWork>& list, bool isBuild, bool needDelete)
{
	if (isBuild == true && inListBuild == false)
	{
		list.push_back({ isBuild, needDelete, this, group });
		isReady = false;
		inListBuild = true;
	}
	else if(inListDestroy == false)
	{
		list.push_back({ isBuild, needDelete, this, group });
		group = NULL;
		inListDestroy = true;
	}

	OutList(list, !isBuild);
}

void ChunkOctreeNode::OutList(std::vector<GPUWork>& list, bool isBuild)
{
	if (isBuild)
	{
		if (inListBuild == false)
		{
			return;
		}
	}
	else
	{
		if (inListDestroy == false)
		{
			return;
		}
	}

	for (std::vector<GPUWork>::iterator iter = list.begin(); iter != list.end(); iter++)
	{
		if (iter->isBuild == isBuild && iter->node == this)
		{
			if (isBuild)
			{
				inListBuild = false;
			}
			else
			{
				inListDestroy = false;
			}
			list.erase(iter);
			return;
		}
	}
}

bool ChunkOctreeNode::hasChild()
{
	if (!childVisible)
	{
		return false;
	}

	for (int i = 0; i < 8; i++)
	{
		if (child[i] == NULL)
		{
			return false;
		}
	}

	return true;
}

ChunkOctreeNode * ChunkOctreeNode::GetMostLeft()
{
	if (child[0] == NULL)
	{
		return this;
	}
	return child[0] -> GetMostLeft();
}

ChunkOctreeNode * ChunkOctreeNode::GetMostRight()
{
	if (child[7] == NULL)
	{
		return this;
	}
	return child[7] -> GetMostRight();
}
