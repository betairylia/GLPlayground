#include "TextDrawer.h"

TextDrawer::TextDrawer()
{
}

TextDrawer::~TextDrawer()
{
}

void TextDrawer::initText2D(const char * texturePath)
{
	ToolBox::printError();
	textTexture = ToolBox::CreateTexture(texturePath);

	ToolBox::printError();
	glGenVertexArrays(1, &vao_textMesh);
	glBindVertexArray(vao_textMesh);

	glGenBuffers(1, &vbo_textMeshPos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_textMeshPos);
	ToolBox::printError();
	glBufferData(GL_ARRAY_BUFFER, 512 * sizeof(glm::vec2), NULL, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	ToolBox::printError();

	glGenBuffers(1, &vbo_textMeshUV);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_textMeshUV);
	glBufferData(GL_ARRAY_BUFFER, 512 * sizeof(glm::vec2), NULL, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	const char* vertex_shader = ToolBox::filetobuf("text.vert");
	const char* fragment_shader = ToolBox::filetobuf("text.frag");

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	textRenderProgram = glCreateProgram();
	glAttachShader(textRenderProgram, fs);
	glAttachShader(textRenderProgram, vs);
	glLinkProgram(textRenderProgram);

	glUseProgram(textRenderProgram);
	glUniform1i(glGetUniformLocation(textRenderProgram, "textTexture"), 0);

	glUniform2i(glGetUniformLocation(textRenderProgram, "windowSizeDiv2"), 800, 450);

	ToolBox::CheckShaderCompiling("textDrawer-Vert", vs);
	ToolBox::CheckShaderCompiling("textDrawer-Frag", fs);

	ToolBox::CheckProgramLinking("textDrawer", textRenderProgram);

	ToolBox::printError();
}

void TextDrawer::printText2D(const char * text, int x, int y, int size, int sizeX)
{
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;

	int length = strlen(text);
	for (unsigned int i = 0; i<length; i++) 
	{
		glm::vec2 vertex_up_left = glm::vec2(x + i*sizeX, y + size);
		glm::vec2 vertex_up_right = glm::vec2(x + i*sizeX + sizeX, y + size);
		glm::vec2 vertex_down_right = glm::vec2(x + i*sizeX + sizeX, y);
		glm::vec2 vertex_down_left = glm::vec2(x + i*sizeX, y);

		vertices.push_back(vertex_up_left);
		vertices.push_back(vertex_down_left);
		vertices.push_back(vertex_up_right);

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i] - 32;
		float uv_x = (character % 16) / 16.0f;
		float uv_y = (character / 16) / 8.0f;

		glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
		glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
		glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 8.0f));
		glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 8.0f));

		UVs.push_back(uv_up_left);
		UVs.push_back(uv_down_left);
		UVs.push_back(uv_up_right);

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_textMeshPos);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec2), vertices.data());

	glBindBuffer(GL_ARRAY_BUFFER, vbo_textMeshUV);
	glBufferSubData(GL_ARRAY_BUFFER, 0, UVs.size() * sizeof(glm::vec2), UVs.data());

	glUseProgram(textRenderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textTexture);

	glBindVertexArray(vao_textMesh);

	glDrawArrays(GL_TRIANGLES, 0, length * 6);

	ToolBox::printError();
}

void TextDrawer::cleanupText2D()
{
	glDeleteBuffers(1, &vbo_textMeshPos);
	glDeleteBuffers(1, &vbo_textMeshUV);
	glDeleteVertexArrays(1, &vao_textMesh);
}
