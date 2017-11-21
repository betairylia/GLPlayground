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
};
