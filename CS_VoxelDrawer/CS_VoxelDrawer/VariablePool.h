#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Used to transfer variables between classes.
//UGLY.
class VariablePool
{
public:
	static float lambdax, lambdaz, ax, az, px, pz;
	static int cs_ChunkMeshGeneration_ScaleIndex;

	static glm::vec3 childPos[8];
	static glm::vec3 quarter;

	static int allocatedGroupCount, allocatedGPUGroupCount, LODCount[10];

	static unsigned char heightMap[2048][2048], colorMap[3][2048][2048];

	static int mapBigChunkLenth;

	static float LODLoadDistance[6];
};
