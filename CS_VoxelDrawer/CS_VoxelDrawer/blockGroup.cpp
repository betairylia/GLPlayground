#include "blockGroup.h"

blockGroup::blockGroup()
{
	bufferUpdated = false;
	bufferInited = false;
}

blockGroup::~blockGroup()
{
}

void blockGroup::Init_sinXsinY(float fx, float fz, float px, float pz, float ax, float az, float groupPosX, float groupPosZ)
{
	for (int x = 0; x < 32; x++)
	{
		for (int y = 0; y < 32; y++)
		{
			for (int z = 0; z < 32; z++)
			{
				if (y < 10)
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
	blockGroupPos.y = 0;
	blockGroupPos.z = groupPosZ;

	bufferUpdated = false;
}

void blockGroup::InitBuffers(GLuint _cs)
{
	//compute_program = _cs;

	glGenVertexArrays(1, &cs_vao);
	glBindVertexArray(cs_vao);

	glGenBuffers(1, &blockId_ssbo);
	glGenBuffers(1, &blockPos_ssbo);
	glGenBuffers(1, &instanceCount_ssbo);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, blockId_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * 32768, blockId, GL_DYNAMIC_DRAW);

	//size is fixed at 8192 now.
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, blockPos_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * 8192, NULL, GL_DYNAMIC_DRAW);

	unsigned int zero = 0;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceCount_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint), &zero, GL_STREAM_READ);

	bufferInited = true;
}

//glUseProgram(...) before call this method
void blockGroup::GenerateBuffer()
{
	if (bufferInited)
	{
		const GLuint ssbos[] = { blockId_ssbo, blockPos_ssbo, instanceCount_ssbo };
		glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 3, ssbos);

		//upload blockgroup position into the uniform buffer
		//glUniform3f(0, blockGroupPos.x, blockGroupPos.y, blockGroupPos.z);

		//todo
		glDispatchCompute(4, 4, 4);

		GLenum err;
		if ((err = glGetError()) != GL_NO_ERROR)
		{
			printf("Error while dispatch compute shader!");
		}

		//get instance count
		//if (instanceCount <= 0)
		//{
			/*glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceCount_ssbo);
			GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			memcpy(&instanceCount, p, sizeof(instanceCount));
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);*/
			
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceCount_ssbo);
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int), &instanceCount);
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
//position - normal - tangent - texCoord1&2 / color - instance
void blockGroup::Draw(int vertCount, int instanceAttribIndex, GLint modelMatrixUniformIndex)
{
	if (bufferInited)
	{
		glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), blockGroupPos);

		glUniformMatrix4fv(modelMatrixUniformIndex, 1, GL_FALSE, glm::value_ptr(modelMat));

		glBindBuffer(GL_ARRAY_BUFFER, blockPos_ssbo);
		glEnableVertexAttribArray(instanceAttribIndex);
		glVertexAttribPointer(instanceAttribIndex, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		glVertexAttribDivisor(instanceAttribIndex, 1);

		//TODO: use glDrawArraysIndirect instead.
		glDrawArraysInstanced(GL_TRIANGLES, 0, vertCount, instanceCount);
		
	}
	else
	{
		printf("Error! buffer not generated before drawing!\n");
	}
}

int blockGroup::getPos(int x, int y, int z)
{
	return x * 32 * 32 + y * 32 + z;
}
