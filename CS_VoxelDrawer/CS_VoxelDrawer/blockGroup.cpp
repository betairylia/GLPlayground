#include "blockGroup.h"

blockGroup::blockGroup(bool useMesh, float _scale)
{
	bufferUpdated = false;
	bufferInited = false;

	useMeshInsteadOfInstanceCube = useMesh;
	scale = _scale;

	VariablePool::allocatedGroupCount++;
}

blockGroup::~blockGroup()
{
	if (buffersFreed == false)
	{
		printf("Buffers are not freed before dtor!\n");
	}

	VariablePool::allocatedGroupCount--;
}

void blockGroup::Init_sinXsinY(float lambdax, float lambdaz, float px, float pz, float ax, float az, float groupPosX, float groupPosZ, float groupPosY)
{
	memset(blockId, 0, sizeof(blockId));

	for (int x = 0; x < 32; x++)
	{
		for (int y = 0; y < 32; y++)
		{
			for (int z = 0; z < 32; z++)
			{
				//if (y < (15 + ax * sinf(((float)x + groupPosX + px) / lambdax * 2 * 3.1415926f)) && 
				//	y < (15 + az * sinf(((float)z + groupPosZ + pz) / lambdaz * 2 * 3.1415926f)))

				if ((groupPosY + (y * scale)) < 
						15 + 
						ax * 
						(sinf(((float)x * scale + groupPosX + px) / lambdax * 1 * 3.1415926f)) *
						(sinf(((float)z * scale + groupPosZ + pz) / lambdaz * 1 * 3.1415926f)) +
						0.5f * ax *
						(sinf(((float)x * scale + groupPosX + px + 10) / lambdax * 0.5 * 3.1415926f)) *
						(sinf(((float)z * scale + groupPosZ + pz +  5) / lambdaz * 0.5 * 3.1415926f)))
				//if(groupPosY + (y * scale) < 16)
				{
					blockId[getPos(x, y, z)] = 1;
				}
				else
				{
					blockId[getPos(x, y, z)] = 0;
				}
			}
		}
	}

	blockGroupPos.x = groupPosX;
	blockGroupPos.y = groupPosY;
	blockGroupPos.z = groupPosZ;

	bufferUpdated = false;
}

void blockGroup::InitHeightColorMaps(float mapScale, float mapScaleY, float groupPosX, float groupPosZ, float groupPosY)
{
	memset(blockId, 0, sizeof(blockId));

	for (int x = 0; x < 32; x++)
	{
		for (int y = 0; y < 32; y++)
		{
			for (int z = 0; z < 32; z++)
			{
				int mX = (int)((x * scale + groupPosX) / mapScale);
				int mZ = (int)((z * scale + groupPosZ) / mapScale);

				if ((groupPosY + (y * scale)) < (mapScaleY * VariablePool::heightMap[mX][mZ]))
				{
					blockId[getPos(x, y, z)] = 
						((int)(VariablePool::colorMap[0][mX][mZ]) << 16) + 
						((int)(VariablePool::colorMap[1][mX][mZ]) <<  8) +
						((int)(VariablePool::colorMap[2][mX][mZ]));
				}
				else
				{
					blockId[getPos(x, y, z)] = 0;
				}

				if ((groupPosY + (y * scale)) < 8)
				{
					blockId[getPos(x, y, z)] = (102 << 8) + 204; //(0, 102, 204), "Ocean blue"
				}
			}
		}
	}

	blockGroupPos.x = groupPosX;
	blockGroupPos.y = groupPosY;
	blockGroupPos.z = groupPosZ;

	bufferUpdated = false;
}

void blockGroup::InitBuffers(GLuint _cs)
{
	glGenVertexArrays(1, &cs_vao);
	glBindVertexArray(cs_vao);

	glGenBuffers(1, &blockId_ssbo);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, blockId_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * 32768, blockId, GL_DYNAMIC_DRAW);

	indirectBufferIndex = IndirectBufferAllocator::GetSingleton()->GetNewIndirectBuffer(this->blockGroupPos, this->scale);

	if (useMeshInsteadOfInstanceCube)
	{
		glGenVertexArrays(1, &mesh_vao);
		glBindVertexArray(mesh_vao);

		glGenBuffers(1, &vertPos_vbo);
		glGenBuffers(1, &vertNormal_vbo);
		glGenBuffers(1, &vertProp_vbo);

		int size = 65536;

		glBindBuffer(GL_ARRAY_BUFFER, vertPos_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * size, NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, vertNormal_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * size, NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, vertProp_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::ivec4) * size, NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	}
	else
	{
		glGenBuffers(1, &blockInstance_ssbo);
		glGenBuffers(1, &blockProp_ssbo);

		//size is fixed at 8192 now.
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, blockInstance_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * 8192, NULL, GL_DYNAMIC_DRAW);

		//size is fixed at 8192 now.
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, blockProp_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::ivec4) * 8192, NULL, GL_DYNAMIC_DRAW);
	}

	bufferInited = true;

	VariablePool::allocatedGPUGroupCount++;
}

void blockGroup::FreeBuffers()
{
	if (useMeshInsteadOfInstanceCube)
	{
		GLuint buf[] = { blockId_ssbo, vertPos_vbo, vertNormal_vbo, vertProp_vbo };
		glDeleteBuffers(5, buf);
		glDeleteVertexArrays(1, &cs_vao);
		glDeleteVertexArrays(1, &mesh_vao);

		IndirectBufferAllocator::GetSingleton()->FreeIndirectBuffer(indirectBufferIndex);
	}

	buffersFreed = true;
	bufferInited = false;
	bufferUpdated = false;

	VariablePool::allocatedGPUGroupCount--;
}

//glUseProgram(...) before call this method
void blockGroup::GenerateBuffer(bool uploadBuffers, int computeShaderScaleIndex)
{
	if (bufferInited)
	{
		//upload buffers if required
		if (uploadBuffers)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, blockId_ssbo);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(blockId), blockId);
		}

		IndirectBufferAllocator::GetSingleton()->RefreshBufferData(indirectBufferIndex);

		if (useMeshInsteadOfInstanceCube)
		{
			const GLuint ssbos[] = { blockId_ssbo, vertPos_vbo, vertNormal_vbo, vertProp_vbo, IndirectBufferAllocator::GetSingleton()->indirectBufferObj };
			glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 5, ssbos);
		}
		else
		{
			const GLuint ssbos[] = { blockId_ssbo, blockInstance_ssbo, blockProp_ssbo, IndirectBufferAllocator::GetSingleton()->indirectBufferObj };
			glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 4, ssbos);
		}

		//upload blockgroup position into the uniform buffer
		//glUniform3f(0, blockGroupPos.x, blockGroupPos.y, blockGroupPos.z);

		//todo
		glDispatchCompute(4, 4, 4);

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL error while Generate Buffer: " << err << std::endl;
		}

		//get instance count
		//if (instanceCount <= 0)
		//{
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, indirectBuffer_ssbo);
			//GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			//memcpy(&cmd, p, sizeof(DrawArraysIndirectCommand));
			//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceCount_ssbo);
			//glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int), &instanceCount);
		//}

		bufferUpdated = true;
	}
	else
	{
		printf("Error! buffer not generated before updating!\n");
	}
}

//glUseProgram(...) before call this method
//glBindVertexArray(...) before call this method
void blockGroup::Draw(int vertCount, int instanceAttribIndex, GLint modelMatrixUniformIndex)
{
	if (bufferInited)
	{
		glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), blockGroupPos);

		glUniformMatrix4fv(modelMatrixUniformIndex, 1, GL_FALSE, glm::value_ptr(modelMat));

		if (useMeshInsteadOfInstanceCube)
		{
			ToolBox::printError();
			glBindVertexArray(mesh_vao);
			ToolBox::printError();
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer_ssbo);

			ToolBox::printError();
			glDrawArraysIndirect(GL_TRIANGLES, (void *)0);
			ToolBox::printError();
		}
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, blockInstance_ssbo);
			glEnableVertexAttribArray(instanceAttribIndex);
			glVertexAttribPointer(instanceAttribIndex, 4, GL_FLOAT, GL_FALSE, 0, NULL);
			glVertexAttribDivisor(instanceAttribIndex, 1);

			glBindBuffer(GL_ARRAY_BUFFER, blockProp_ssbo);
			glEnableVertexAttribArray(instanceAttribIndex + 1);
			glVertexAttribPointer(instanceAttribIndex + 1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
			glVertexAttribDivisor(instanceAttribIndex + 1, 1);

			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer_ssbo);

			//TODO: use glDrawArraysIndirect instead.
			//glDrawArraysInstanced(GL_TRIANGLES, 0, vertCount, instanceCount);
			glDrawArraysIndirect(GL_TRIANGLES, (void *)0);
		}
	}
	else
	{
		//printf("Error! buffer not generated before drawing!\n");
	}
}

bool blockGroup::ReadyForDraw()
{
	return bufferUpdated;
}

int blockGroup::getPos(int x, int y, int z)
{
	return x * 32 * 32 + y * 32 + z;
}
