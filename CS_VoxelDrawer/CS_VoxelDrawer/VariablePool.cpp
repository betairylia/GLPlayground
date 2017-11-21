#include "VariablePool.h"

glm::vec3 VariablePool::childPos[8] = 
{
	glm::vec3( 0,  0,  0),
	glm::vec3( 0,  0, 16),
	glm::vec3( 0, 16,  0),
	glm::vec3( 0, 16, 16),
	glm::vec3(16,  0,  0),
	glm::vec3(16,  0, 16),
	glm::vec3(16, 16,  0),
	glm::vec3(16, 16, 16)
};

glm::vec3 VariablePool::quarter = glm::vec3(8);

float VariablePool::lambdax, VariablePool::lambdaz, VariablePool::px, VariablePool::pz, VariablePool::ax, VariablePool::az;
int VariablePool::cs_ChunkMeshGeneration_ScaleIndex;

int VariablePool::allocatedGroupCount = 0, VariablePool::allocatedGPUGroupCount = 0, VariablePool::LODCount[10] = {};
