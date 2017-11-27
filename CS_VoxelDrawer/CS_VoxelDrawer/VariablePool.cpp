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

unsigned char VariablePool::heightMap[2048][2048];
unsigned char VariablePool::colorMap[3][2048][2048];

int VariablePool::mapBigChunkLenth = 2;

float VariablePool::LODLoadDistance[6] = { 192.0f, 256.0f, 512.0f, 1024.0f, 2048.0f, 8192.0f };
//float VariablePool::LODLoadDistance[6] = { 192.0f, 256.0f, 384.0f, 768.0f, 1024.0f, 8192.0f };
//float VariablePool::LODLoadDistance[6] = {256.0f, 384.0f, 384.0f, 512.0f, 1024.0f, 8192.0f};
