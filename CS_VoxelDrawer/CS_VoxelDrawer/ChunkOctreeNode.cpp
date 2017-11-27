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
	inLinkedList = false;
}

ChunkOctreeNode::~ChunkOctreeNode()
{
	for (int i = 0; i < 8; i++)
	{
		if (child[i] != NULL)
		{
			child[i]->father = NULL;
		}
	}

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
	//group->Init_sinXsinY(
	//	VariablePool::lambdax, VariablePool::lambdaz,
	//	VariablePool::px, VariablePool::pz,
	//	VariablePool::ax, VariablePool::az,
	//	pos.x, pos.z, pos.y);

	group->InitHeightColorMaps(
		VariablePool::mapBigChunkLenth / 2,
		1.0f,
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
	if (isBuild == false && inListDestroy == false)
	{
		list.push_back({ isBuild, needDelete, this, group });
		inListDestroy = true;
	}

	OutList(list, !isBuild);
}

void ChunkOctreeNode::OutList(std::vector<GPUWork>& list, bool isBuild, bool isDelete)
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
				if (isDelete && !(iter->needDelete == isDelete))
				{
					continue;
				}
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

void ChunkOctreeNode::SelfInLinkedList()
{
	bool mask = GetMostLeft()->inLinkedList;

	_SetSubTreeInLinkedList(this, false);

	inLinkedList = true & mask;
}

void ChunkOctreeNode::_SetSubTreeInLinkedList(ChunkOctreeNode *node, bool isIn)
{
	node->inLinkedList = isIn;
	for (int i = 0; i < 8; i++)
	{
		if(node->child[i] != NULL)
			_SetSubTreeInLinkedList(node->child[i], isIn);
	}
}

void ChunkOctreeNode::SubTreeInLinkedList()
{
	bool mask = inLinkedList;
	inLinkedList = false;

	ChunkOctreeNode *l = GetMostLeft(), *r = GetMostRight();

	l->inLinkedList = true & mask;
	while (l != r)
	{
		l = l -> next;
		l->inLinkedList = true & mask;
	}
}

ChunkOctreeNode * ChunkOctreeNode::GetMostLeft()
{
	if (child[0] == NULL || childVisible == false)
	{
		return this;
	}
	return child[0] -> GetMostLeft();
}

ChunkOctreeNode * ChunkOctreeNode::GetMostRight()
{
	if (child[7] == NULL || childVisible == false)
	{
		return this;
	}
	return child[7] -> GetMostRight();
}
