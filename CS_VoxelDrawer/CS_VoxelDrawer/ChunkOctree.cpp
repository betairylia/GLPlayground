#include "ChunkOctree.h"

ChunkOctree::ChunkOctree(std::mutex& _m, std::condition_variable& _cv, bool mt) : m_mutex(_m), m_condVar(_cv), multiThread(mt)
{
	memset(mp_treeRoot, 0, sizeof(mp_treeRoot));
}

ChunkOctree::~ChunkOctree()
{
}

void ChunkOctree::Update(glm::vec3 playerPos)
{
	m_playerPos = playerPos;

	for (int i = 0; i < VariablePool::mapBigChunkLenth; i++)
	{
		for (int j = 0; j < VariablePool::mapBigChunkLenth; j++)
		{
			if (mp_treeRoot[i][j] == NULL)
			{
				mp_treeRoot[i][j] = new ChunkOctreeNode(glm::vec3(i * 1024, 0, j * 1024), glm::vec3(i * 1024 + 512, 512, j * 1024 + 512), 32, NULL, 0);
			}

			if (renderList[i][j].next == NULL)
			{
				//List init
				renderList[i][j].next = mp_treeRoot[i][j];
				mp_treeRoot[i][j]->prev = &renderList[i][j];
				mp_treeRoot[i][j]->next = NULL;

				mp_treeRoot[i][j]->inLinkedList = true;
			}
		
			//UpdateNode(mp_treeRoot[i][j]);

			PreUpdateNode(mp_treeRoot[i][j]);
		}
	}
	DoWork();
	for (int i = 0; i < VariablePool::mapBigChunkLenth; i++)
	{
		for (int j = 0; j < VariablePool::mapBigChunkLenth; j++)
		{
			//printf("Entering PostUpdate\n");
			PostUpdateNode(mp_treeRoot[i][j]);
		}
	}

	//system("pause");
}

//glUseProgram(...) before call this method
//glBindVertexArray(...) before call this method
void ChunkOctree::Drawall_WalkThrough(int vertCount, int instanceAttribIndex, GLint modelMatrixUniformIndex)
{
	for (int i = 0; i < 10; i++)
	{
		VariablePool::LODCount[i] = 0;
	}

	for (int i = 0; i < VariablePool::mapBigChunkLenth; i++)
	{
		for (int j = 0; j < VariablePool::mapBigChunkLenth; j++)
		{
			_DrawNode(mp_treeRoot[i][j], vertCount, instanceAttribIndex, modelMatrixUniformIndex);
		}
	}
}

void ChunkOctree::_DrawNode(ChunkOctreeNode* node, int vertCount, int instanceAttribIndex, GLint modelMatrixUniformIndex)
{
	if (node == NULL)
	{
		return;
	}

	if (node->group != NULL)
	{
		int id = 0, tmp = node->scale >> 1;
		while (tmp > 0) { tmp >>= 1; id++; }
		VariablePool::LODCount[id]++;

		node->group->Draw(vertCount, instanceAttribIndex, modelMatrixUniformIndex);
	}

	if (node->group == NULL && node->hasChild())
	{
		for (int i = 0; i < 8; i++)
		{
			_DrawNode(node->child[i], vertCount, instanceAttribIndex, modelMatrixUniformIndex);
		}
	}
}

void ChunkOctree::PreUpdateNode(ChunkOctreeNode * node)
{
	//Should this node be expanded ? Does it has any children ?
	bool expand = false, hasChild = false;

	//Should this node be loaded and rendered ? (due to too large scale etc.)
	bool load = true;

	//The node can only expand when its scale is larger than 1.
	if (node->scale > 1)
	{
		expand = false;
		hasChild = node->hasChild();

		//calculate the destiny (?) of this node.
		float dist = glm::distance(m_playerPos, node->centerPos);

		//TODO: different load distance between different scale
		//dist /= (float)node->scale;

		//Distence to expand, DIVIDED BY 2
		/*float tmp_loadDist = 192.0f;
		if (dist <= tmp_loadDist)
		{
			expand = true;
		}*/

		int id = 0, s = (node->scale >> 1);
		while (s > 0) { id++; s >>= 1; }

		if (dist <= VariablePool::LODLoadDistance[id])
		{
			expand = true;
		}

		//Do not load chunks bigger than scale 2
		/*if (node->scale > 2)
		{
			load = false;
		}*/
	}

	if (node->pos.y >= 256)
	{
		load = false;
	}

	node->needExpand = expand;
	node->hadChild = hasChild;

	//Do the operation
	//Already expanded, nothing to do. Passing to children.
	if (expand && hasChild)
	{
		for (int i = 0; i < 8; i++)
		{
			PreUpdateNode(node->child[i]);
		}
	}
	//The node needs to be expanded and recreate children.
	//And it MUST be a leaf node since the update process before.
	else if (expand && !hasChild)
	{
		bool doLinkListWork = false;

		//May be we need this...
		if (!node->inLinkedList)
		{
			node->childVisible = true;

			if (node->hasChild())
			{
				//List update
				node->SelfInLinkedList();
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
				if (node->group != NULL)
				{
					node->InList(workList, false, false);
				}
 			}
			else
			{
				doLinkListWork = true;
			}
		}
	
		node->childVisible = true;
		node->isReady = false;

		//Init nodes
		for (int i = 0; i < 8; i++)
		{
			if (node->child[i] == NULL)
			{
				glm::vec3 cPos = node->pos + (float)node->scale * VariablePool::childPos[i];
				node->child[i] = new ChunkOctreeNode(cPos, cPos + ((float)node->scale * VariablePool::quarter), node->scale / 2, node, i);
			}
			node->child[i]->isReady = false;
			node->child[i]->OutList(workList, false, true);
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
			//In PreUpdate: Allocation, Create local list and Register into workList.
			PreUpdateNode(node->child[i]);
		}

		if (doLinkListWork)
		{
			node->SubTreeInLinkedList();

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
		}

		node->OutList(workList, true);
	}
	//The node is leaf node
	//It may needs to be narrowed ("fall back", collapase).
	else if (!expand)
	{
		//TODO: collect if the group has not been destroyed
		//If it has any child, it should be narrowed and reconstructed.
		if (hasChild)
		{
			StopChildLoading(node);
			if (node->inLinkedList)
			{
				if (node->child[0]->inLinkedList)
				{
					//printf("!!! ERROR PLEASE CHECK\n");
				}
				node->childVisible = false;
			}
			node->isReady = false;
		}

		//Construct self data
		if (node->isReady == false)
		{
			if (load)
			{
				node->InList(workList, true, false);
			}
			else
			{
				node->isReady = true;

				//List update
				if (node->inLinkedList == false)
				{
					node->SelfInLinkedList();

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
				}

				//Destroy children
				CleanChildResc(node);
			}
		}
	}

	node->isReadyPrev = node->isReady;
}

bool ChunkOctree::PostUpdateNode(ChunkOctreeNode * node)
{
	bool expand = node->needExpand;
	bool hasChild = node->hadChild;
	bool readyBefore = node->isReadyPrev;

	//The node is not a leaf.
	if (expand)
	{
		if (node->isReady == false)
		{
			//Update ready state
			node->isReady = true;
			for (int i = 0; i < 8; i++)
			{
				node->isReady &= PostUpdateNode(node->child[i]);
			}

			//Only update if this node is ready.
			if (node->isReady)
			{
				//If this node become ready in this update, then we need update its children.
				if (!readyBefore)
				{
					//printf("Expand:   (%d, %d, %d)\tx%d\n", (int)node->pos.x, (int)node->pos.y, (int)node->pos.z, node->scale);

					//List update
					if (node->inLinkedList == true)
					{
						node->SubTreeInLinkedList();

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
					}

					//Self destruct
					node->InList(workList, false, false);
				}

				//Tell others that this node is ready.
				return true;
			}
		}
		else
		{
			for (int i = 0; i < 8; i++)
			{
				PostUpdateNode(node->child[i]);
			}
			return true;
		}
	}
	//The node is leaf node
	//It may needs to be narrowed ("fall back", collapase).
	else
	{
		if (node->isReady)
		{
			//Cleaning
			if (!readyBefore)
			{
				//printf("Collpase: (%d, %d, %d)\tx%d\n", (int)node->pos.x, (int)node->pos.y, (int)node->pos.z, node->scale);

				//if (node->inLinkedList && node->child[0]->inLinkedList)
				//{
				//	printf("!!! ERROR PLEASE CHECK\n");
				//}

				//List update
				if (node->inLinkedList == false)
				{
					node->SelfInLinkedList();

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
				}

				//Destroy children
				CleanChildResc(node);
			}

			//Tell others this node is ready.
			return true;
		}
	}

	//This node is not ready.
	return false;
}

void ChunkOctree::CleanChildResc(ChunkOctreeNode * node)
{
	for (int i = 0; i < 8; i++)
	{
		if (node->child[i] != NULL)
		{
			CleanChildResc(node->child[i]);
			node->child[i]->InList(workList, false, true);
		}
	}
	//TODO: Reusing
	node->childVisible = false;
}

void ChunkOctree::StopChildLoading(ChunkOctreeNode * node)
{
	for (int i = 0; i < 8; i++)
	{
		if (node->child[i] != NULL)
		{
			StopChildLoading(node->child[i]);
			node->child[i]->OutList(workList, true);
		}
	}
}

void ChunkOctree::DoWork()
{
	int len = workList.size();
	//Clean the workList First.
	for (int i = 0; i < len; i++)
	{
		if(workList.at(i).isBuild == true && workList.at(i).node->group != NULL)
		{
			workList.at(i).node->isReady = true;
			workList.at(i).node->inListBuild = false;
			workList.erase(workList.begin() + i);
			i--;
			len--;
			continue;
		}
		if (workList.at(i).isBuild == false && workList.at(i).node->group == NULL)
		{
			workList.at(i).node->inListDestroy = false;
			workList.at(i).node->isReady = false;

			if (workList.at(i).needDelete == true)
			{
				delete workList.at(i).node;
			}
			workList.erase(workList.begin() + i);
			i--;
			len--;
			continue;
		}
	}

	//Sort the workList
	std::sort(workList.begin(), workList.end(), [](const ChunkOctreeNode::GPUWork& lhs, const ChunkOctreeNode::GPUWork& rhs)
	{
		int lValue = lhs.isBuild ? 0 : 33, rValue = rhs.isBuild ? 0 : 33;
		lValue += lhs.node->scale;
		rValue += rhs.node->scale;

		return lValue > rValue;
	});

	len = workList.size() > 128 ? 128 : workList.size();
	//len = workList.size();
	if (len == 0) return;

	//Only CPU Part can use parallel computing.
	for (int i = 0; i < len; i++)
	{
		if (workList.at(i).isBuild == true)
		{
			workList.at(i).node->CreateGroup();
		}
		//else
		//{
		//	workList.at(i).node->isReady = false;
		//}
	}

#pragma omp parallel for num_threads(16)
	for (int i = 0; i < len; i++)
	{
		if (workList.at(i).isBuild == true)
		{
			workList.at(i).node->BuildGroupData();
		}
	}

	for (int i = 0; i < len; i++)
	{
		GPUworkList.push_back(workList.at(i));
	}

	if (multiThread)
	{
		//This is GPU Part.
		//GPU Computation should send to the main thread to compute.
		//Just wait for it finishes.

		//printf("INFO:\tWaiting for main thread...\n");

		std::unique_lock<std::mutex> mlock(m_mutex);
		m_condVar.wait(mlock, [this]() {return (GPUworkList.size() <= 0);});

		//printf("INFO:\tGPU Calculation finished!\n");

		GPUworkList.clear();
	}
	else
	{
		glUseProgram(compute_programme);
		printf("%d\n", len);
		for (int i = 0; i < len; i++)
		{
			if (GPUworkList.at(i).isBuild == true)
			{
				GPUworkList.at(i).node->InitGroupMesh();
				GPUworkList.at(i).node->BuildGroupMesh();
			}
			else
			{
				if (GPUworkList.at(i).groupBak != NULL)
				{
					GPUworkList.at(i).groupBak->FreeBuffers();
				}
			}
		}
	}

	for (int i = 0; i < len; i++)
	{
		if (workList.at(i).isBuild == false)
		{
			workList.at(i).node->group = NULL;
			delete workList.at(i).groupBak;
			workList.at(i).node->inListDestroy = false;
			workList.at(i).node->isReady = false;

			if (workList.at(i).needDelete == true)
			{
				delete workList.at(i).node;
			}
		}
		else
		{
			workList.at(i).node->inListBuild = false;

			workList.at(i).node->isReady = true;
			//workList.at(i).node->groupReady = true;
		}
	}

	//Clear work list of lenth = len.
	workList.erase(workList.begin(), workList.begin() + len);
}

void ChunkOctree::UpdateNode(ChunkOctreeNode * node)
{
	////Should this node be expanded ? Does it has any children ?
	//bool expand = false, hasChild = false;

	////Should this node be loaded and rendered ? (due to too large scale etc.)
	//bool load = true;

	////The node can only expand when its scale is larger than 1.
	//if (node->scale > 1)
	//{
	//	expand = false;
	//	hasChild = (node->child[0] != NULL);

	//	//calculate the destiny (?) of this node.
	//	float dist = glm::distance(m_playerPos, node->centerPos);

	//	//TODO: different load distance between different scale
	//	dist /= (float)node->scale;

	//	//Distence to expand, DIVIDED BY 2
	//	float tmp_loadDist = 96.0f;
	//	if (dist <= tmp_loadDist)
	//	{
	//		expand = true;
	//	}

	//	//Do not load chunks bigger than scale 2
	//	//if (node->scale > 2)
	//	//{
	//	//	load = false;
	//	//}
	//}

	////Do the operation
	////Already expanded, nothing to do. Passing to children.
	//if (expand && hasChild)
	//{
	//	for (int i = 0; i < 8; i++)
	//	{
	//		UpdateNode(node->child[i]);
	//	}
	//}
	////The node needs to be expanded.
	//else if (expand && !hasChild)
	//{
	//	//Init nodes
	//	for (int i = 0; i < 8; i++)
	//	{
	//		glm::vec3 cPos = node->pos + (float)node->scale * VariablePool::childPos[i];
	//		node->child[i] = new ChunkOctreeNode(cPos, cPos + VariablePool::quarter, node->scale / 2);
	//	}

	//	//Build local list
	//	for (int i = 0; i < 8; i++)
	//	{
	//		node->child[i]->prev = i > 0 ? node->child[i - 1] : NULL;
	//		node->child[i]->next = i < 7 ? node->child[i + 1] : NULL;
	//	}

	//	//Update nodes
	//	for (int i = 0; i < 8; i++)
	//	{
	//		//Make sure all the child node is finalized
	//		UpdateNode(node->child[i]);
	//	}

	//	//Since we must make sure that the render list is always aviliable,
	//	//we should destroy the node after list updated.

	//	//List update
	//	ChunkOctreeNode *l = node->GetMostLeft(), *r = node->GetMostRight();
	//	l->prev = node->prev;
	//	if (node->prev != NULL)
	//	{
	//		node->prev->next = l;
	//	}
	//	r->next = node->next;
	//	if (node->next != NULL)
	//	{
	//		node->next->prev = r;
	//	}

	//	//Self destruct
	//	node->ClearGroup();
	//}
	////The node is leaf node
	////It may needs to be narrowed ("fall back", collapase).
	//else if (!expand)
	//{
	//	//Construct self data
	//	if (node->group == NULL && load)
	//	{
	//		node->BuildGroupData();
	//		node->InitGroupMesh();
	//		node->BuildGroupMesh();
	//	}

	//	//Cleaning
	//	if (hasChild)
	//	{
	//		//List update
	//		ChunkOctreeNode *l = node->GetMostLeft(), *r = node->GetMostRight();
	//		node->prev = l->prev;
	//		if (l->prev != NULL)
	//		{
	//			l->prev->next = node;
	//		}
	//		node->next = r->next;
	//		if (r->next != NULL)
	//		{
	//			r->next->prev = node;
	//		}

	//		//Destroy children
	//		nodeCleanChildResc();
	//	}
	//}
}
