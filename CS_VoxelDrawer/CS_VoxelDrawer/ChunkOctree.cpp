#include "ChunkOctree.h"

ChunkOctree::ChunkOctree()
{
	memset(mp_treeRoot, 0, sizeof(mp_treeRoot));
}

ChunkOctree::~ChunkOctree()
{
}

void ChunkOctree::Update(glm::vec3 playerPos)
{
	m_playerPos = playerPos;

	for (int i = 0; i < mapBigChunkLenth; i++)
	{
		for (int j = 0; j < mapBigChunkLenth; j++)
		{
			if (mp_treeRoot[i][j] == NULL)
			{
				mp_treeRoot[i][j] = new ChunkOctreeNode(glm::vec3(i * 1024, 0, j * 1024), glm::vec3(i * 1024 + 512, 512, j * 1024 + 512), 32);
				
				//List init
				renderList[i][j].next = mp_treeRoot[i][j];
				mp_treeRoot[i][j]->prev = &renderList[i][j];
				mp_treeRoot[i][j]->next = NULL;	
			}
		
			UpdateNode(mp_treeRoot[i][j]);
		}
	}
}

//glUseProgram(...) before call this method
//glBindVertexArray(...) before call this method
void ChunkOctree::Drawall(int vertCount, int instanceAttribIndex, GLint modelMatrixUniformIndex)
{
	for (int i = 0; i < mapBigChunkLenth; i++)
	{
		for (int j = 0; j < mapBigChunkLenth; j++)
		{
			ChunkOctreeNode* pRender = renderList[i][j].next;

			//printf("=======Start frame=======\n");
			
			//Go through the render list
			while (pRender != NULL)
			{
				//printf("Drawing: %d\t%d\t%d\tx%d\n", (int)pRender->pos.x, (int)pRender->pos.y, (int)pRender->pos.z, pRender->scale);
				//Simply draw them all
				if (pRender->group != NULL)
				{
					//printf("Drawing: %d\t%d\t%d\tx%d\n", (int)pRender->pos.x, (int)pRender->pos.y, (int)pRender->pos.z, pRender->scale);
					pRender->group->Draw(vertCount, instanceAttribIndex, modelMatrixUniformIndex);
				}
				pRender = pRender->next;
			}
		}
	}
}

void ChunkOctree::UpdateNode(ChunkOctreeNode * node)
{
	//Should this node be expanded ? Does it has any children ?
	bool expand = false, hasChild = false;

	//Should this node be loaded and rendered ? (due to too large scale etc.)
	bool load = true;

	//The node can only expand when its scale is larger than 1.
	if (node->scale > 1)
	{
		expand = false;
		hasChild = (node->child[0] != NULL);

		//calculate the destiny (?) of this node.
		float dist = glm::distance(m_playerPos, node->centerPos);

		//TODO: different load distance between different scale
		dist /= (float)node->scale;

		//Distence to expand, DIVIDED BY 2
		float tmp_loadDist = 96.0f;
		if (dist <= tmp_loadDist)
		{
			expand = true;
		}

		//Do not load chunks bigger than scale 2
		//if (node->scale > 2)
		//{
		//	load = false;
		//}
	}

	//Do the operation
	//Already expanded, nothing to do. Passing to children.
	if (expand && hasChild)
	{
		for (int i = 0; i < 8; i++)
		{
			UpdateNode(node->child[i]);
		}
	}
	//The node needs to be expanded.
	else if (expand && !hasChild)
	{
		//Init nodes
		for (int i = 0; i < 8; i++)
		{
			glm::vec3 cPos = node->pos + (float)node->scale * VariablePool::childPos[i];
			node->child[i] = new ChunkOctreeNode(cPos, cPos + VariablePool::quarter, node->scale / 2);
		}

		//Build local list
		for (int i = 0; i < 8; i++)
		{
			node->child[i]->prev = i > 0 ? node->child[i - 1] : NULL;
			node->child[i]->next = i < 7 ? node->child[i + 1] : NULL;
		}

		//Update nodes
		for (int i = 0; i < 8; i++)
		{
			//Make sure all the child node is finalized
			UpdateNode(node->child[i]);
		}

		//Since we must make sure that the render list is always aviliable,
		//we should destroy the node after list updated.

		//List update
		ChunkOctreeNode *l = node->GetMostLeft(), *r = node->GetMostRight();
		l->prev = node->prev;
		if (node->prev != NULL)
		{
			node->prev->next = l;
		}
		r->next = node->next;
		if (node->next != NULL)
		{
			node->next->prev = r;
		}

		//Self destruct
		node->ClearGroup();
	}
	//The node is leaf node
	//It may needs to be narrowed ("fall back", collapase).
	else if (!expand)
	{
		//Construct self data
		if (node->group == NULL && load)
		{
			node->BuildGroup();
		}

		//Cleaning
		if (hasChild)
		{
			//List update
			ChunkOctreeNode *l = node->GetMostLeft(), *r = node->GetMostRight();
			node->prev = l->prev;
			if (l->prev != NULL)
			{
				l->prev->next = node;
			}
			node->next = r->next;
			if (r->next != NULL)
			{
				r->next->prev = node;
			}

			//Destroy children
			node->CleanChildResc();
		}
	}
}
