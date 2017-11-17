#pragma once

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

//Used to transfer variables between classes.
//UGLY.
class VariablePool
{
public:
	static float lambdax, lambdaz, ax, az, px, pz;
	static int cs_ChunkMeshGeneration_ScaleIndex;

	static glm::vec3 childPos[8];
	static glm::vec3 quarter;
};
