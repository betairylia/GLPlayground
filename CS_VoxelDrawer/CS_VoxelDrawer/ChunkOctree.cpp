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
			}
		
			//UpdateNode(mp_treeRoot[i][j]);

			PreUpdateNode(mp_treeRoot[i][j]);
		}
	}
	DoWork();
	//for (int i = 0; i < VariablePool::mapBigChunkLenth; i++)
	//{
	//	for (int j = 0; j < VariablePool::mapBigChunkLenth; j++)
	//	{
	//		//printf("Entering PostUpdate\n");
	//		PostUpdateNode(mp_treeRoot[i][j]);
	//	}
	//}

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

	if (node->group != NULL && node->group->ReadyForDraw())
	{
		int id = 0, tmp = node->scale >> 1;
		while (tmp > 0) { tmp >>= 1; id++; }
		VariablePool::LODCount[id]++;

		node->group->Draw(vertCount, instanceAttribIndex, modelMatrixUniformIndex);
	}
	else if (node->hasChild())
	{
		for (int i = 0; i < 8; i++)
		{
			_DrawNode(node->child[i], vertCount, instanceAttribIndex, modelMatrixUniformIndex);
		}
	}
}

bool ChunkOctree::PreUpdateNode(ChunkOctreeNode * node)
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

	//Do the operation
	//Already expanded, nothing to do. Passing to children.
	if (expand && hasChild)
	{
		bool ready = true;
		for (int i = 0; i < 8; i++)
		{
			ready &= PreUpdateNode(node->child[i]);
		}

		if (ready && node->group != NULL)
		{
			//self destruct
			node->InList(workList, false, false);
		}

		return ready;
	}
	//The node needs to be expanded and recreate children.
	//And it MUST be a leaf node since the update process before.
	else if (expand && !hasChild)
	{
		//Init nodes
		for (int i = 0; i < 8; i++)
		{
			if (node->child[i] == NULL)
			{
				glm::vec3 cPos = node->pos + (float)node->scale * VariablePool::childPos[i];
				node->child[i] = new ChunkOctreeNode(cPos, cPos + ((float)node->scale * VariablePool::quarter), node->scale / 2, node, i);
			}
			node->child[i]->OutList(workList, false, true);
		}

		//Update nodes
		for (int i = 0; i < 8; i++)
		{
			//Make sure all the child node is finalized
			//In PreUpdate: Allocation, Create local list and Register into workList.
			PreUpdateNode(node->child[i]);
		}

		node->OutList(workList, true);

		return false;
	}
	//The node is leaf node
	//It may needs to be narrowed ("fall back", collapase).
	else if (!expand)
	{
		//TODO: collect if the group has not been destroyed
		//If it has any child, it should be narrowed and reconstructed.

		//Construct self data
		if (node->group == NULL)
		{
			if (load)
			{
				node->InList(workList, true, false);
			}
			else
			{
				node->isEmpty = true;
			}
		}
		else if(node->group->ReadyForDraw())
		{
			CleanChildResc(node);
		}

		return (node->isEmpty || node->group != NULL);
	}
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
			workList.at(i).node->inListBuild = false;
			workList.erase(workList.begin() + i);
			i--;
			len--;
			continue;
		}
		if (workList.at(i).isBuild == false && workList.at(i).node->group == NULL)
		{
			workList.at(i).node->inListDestroy = false;

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

			if (workList.at(i).needDelete == true)
			{
				delete workList.at(i).node;
			}
		}
		else
		{
			workList.at(i).node->inListBuild = false;
		}
	}

	//Clear work list of lenth = len.
	workList.erase(workList.begin(), workList.begin() + len);
}
