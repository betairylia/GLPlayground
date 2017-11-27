#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <gl/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <iostream>

#include <thread>

#include "ChunkOctree.h"
#include "TextDrawer.h"

GLfloat cube_positions[] = 
{
	 0.0f, 0.0f, 0.0f, 1.0f, // triangle 1 : begin
	 0.0f, 0.0f, 1.0f, 1.0f,
	 0.0f, 1.0f, 1.0f, 1.0f, // triangle 1 : end

	 1.0f, 1.0f, 0.0f, 1.0f, // triangle 2 : begin
	 0.0f, 0.0f, 0.0f, 1.0f,
	 0.0f, 1.0f, 0.0f, 1.0f, // triangle 2 : end

	 1.0f, 0.0f, 1.0f, 1.0f,
	 0.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 0.0f, 1.0f,

	 1.0f, 1.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 0.0f, 1.0f,

	 0.0f, 0.0f, 0.0f, 1.0f,
	 0.0f, 1.0f, 1.0f, 1.0f,
	 0.0f, 1.0f, 0.0f, 1.0f,

	 1.0f, 0.0f, 1.0f, 1.0f,
	 0.0f, 0.0f, 1.0f, 1.0f,
	 0.0f, 0.0f, 0.0f, 1.0f,

	 0.0f, 1.0f, 1.0f, 1.0f,
	 0.0f, 0.0f, 1.0f, 1.0f,
	 1.0f, 0.0f, 1.0f, 1.0f,

	 1.0f, 1.0f, 1.0f, 1.0f,
	 1.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 1.0f, 0.0f, 1.0f,

	 1.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 1.0f, 1.0f, 1.0f,
	 1.0f, 0.0f, 1.0f, 1.0f,

	 1.0f, 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 0.0f, 1.0f,
	 0.0f, 1.0f, 0.0f, 1.0f,

	 1.0f, 1.0f, 1.0f, 1.0f,
	 0.0f, 1.0f, 0.0f, 1.0f,
	 0.0f, 1.0f, 1.0f, 1.0f,

	 1.0f, 1.0f, 1.0f, 1.0f,
	 0.0f, 1.0f, 1.0f, 1.0f,
	 1.0f, 0.0f, 1.0f, 1.0f
};

GLfloat cube_normals[] =
{
    -1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	 0.0f, 0.0f,-1.0f,
	 0.0f, 0.0f,-1.0f,
	 0.0f, 0.0f,-1.0f,

	 0.0f,-1.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,

	 0.0f, 0.0f,-1.0f,
	 0.0f, 0.0f,-1.0f,
	 0.0f, 0.0f,-1.0f,

	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	 0.0f,-1.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,

	 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f,

	 1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 0.0f,

	 1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 0.0f,

	 0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,

	 0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,

	 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f,
};

GLfloat g_quad_vertex_buffer_data[] = 
{
	-1.0f, -1.0f, 0.0f, 1.0f,
	 1.0f, -1.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 1.0f,
	 1.0f, -1.0f, 0.0f, 1.0f,
	 1.0f,  1.0f, 0.0f, 1.0f,
};

GLuint vao_cube, vbo_position, vbo_normal, vs, fs, cs, shader_programme, compute_programme;
GLuint vao_screenQuad, vbo_screenQuad;
glm::vec3 cameraPos;
glm::mat4 cameraRot;

bool keyState[256] = {};
bool updateEveryFrame = false;
bool useMeshInsteadOfInstanceCube = true;

//advanced pipeline will be ignored if you don't use mesh instead of instance cubes.
bool useAdvancedRenderingPipeline = true;

bool useAnotherThreadForMap = true;

bool testLOD = false;
bool useOctree = true;

/*
Performance (TITAN X - 5960X - 32GB):

								Average FPS			Dispatch Time			Draw Time			Memory Cost / Chunk

InstanceCube:					avg 117FPS			35.618					299.112				256KB
InstanceCube - faceCulling:		avg 123FPS			37.025					284.200				256KB
Compute mesh [Selected]:		avg 138FPS			87.650					52.642				3MB (12x Larger)
								avg 124FPS with SSAO
*/

/*
TODO List:

==Hig==
LOD File I/O
World generator
Rendering pipeline

==Low==
rawIdToMesh shader optimization
Dynamic buffer size allocation
*/

float cameraArc = 1.309f /*75 degrees*/, aspectRatio = 16.0f / 9.0f, cameraNear = 1.0f, cameraFar = 8192.0f;
float prevTime, nowTime;
int avgFPS = 0, minFPS = 999, currentFPS = 0;
int frameCount = 0;

std::vector<blockGroup *> blockGroupList;

int groupCountX = 10, groupCountZ = 10;	
float lambdax = 72, lambdaz = 64, ax = 24, az = 5, px = 3, pz = 12;

int mousePrevX = -1, mousePrevY = -1;
float mouse_dx, mouse_dy;
float cameraRotY = 0.0f, cameraRotX = 0.0f;
float scalarSpeed, spdWalk = 50.0f, spdRun = 800.0f;

//Blockgroup management
std::mutex m_mutex;
std::condition_variable m_condVar;
ChunkOctree chunkOctree(m_mutex, m_condVar, useAnotherThreadForMap);
bool octreeThreadTerminate = false, calculatingChunks = false;
int maximumComputeWorksPerFrame = 128, maximumWorkTimeMs = 8;

int workListLenth;

//Rendering stuffs
const int windowHeight = 900, windowWidth = 1600;

GLuint frameBuffer_MRT, frameBuffer_SSAO = 0;
GLuint RT_Position, RT_Normal, RT_Color, RT_AOMap = 0;
GLuint RT_Depth = 0;
GLuint pipeline_MRT, pipeline_AO;

//Text drawer
TextDrawer textDrawer;

int min(int a, int b)
{
	return a < b ? a : b;
}

int max(int a, int b)
{
	return a > b ? a : b;
}

void printError()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL error: " << err << std::endl;
	}
}

void CheckProgramLinking(char* programName, GLuint program)
{
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		//The maxLength includes the NULL character
		char infolog[1024];
		glGetProgramInfoLog(program, 1024, NULL, infolog);

		printf("The %s shader failed to linking with the error: %s \n", programName, infolog);

		//Provide the infolog in whatever manner you deem best.
		//Exit with failure.
		return;
	}
}

void CheckShaderCompiling(char* shaderName, GLuint shader)
{
	GLint testval = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &testval);
	if (testval == GL_FALSE)
	{
		char infolog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, infolog);
		printf("The %s shader failed to compile with the error: %s \n", shaderName, infolog);
	}
}

/* A simple function that will read a file into an allocated char pointer buffer */
char* filetobuf(char *file)
{
	FILE *fptr;
	long length;
	char *buf;

	fptr = fopen(file, "rb"); /* Open file for reading */
	if (!fptr) /* Return NULL on failure */
		return NULL;
	fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
	length = ftell(fptr); /* Find out how many bytes into the file we are */
	buf = (char*)malloc(length + 1); /* Allocate a buffer for the entire length of the file and a null terminator */
	fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
	fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
	fclose(fptr); /* Close the file */
	buf[length] = 0; /* Null terminator */

	return buf; /* Return the buffer */
}

void setVSync(bool sync)
{
	// Function pointer for the wgl extention function we need to enable/disable
	// vsync
	typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

	bool flag = false;

	GLint n, i;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	for (i = 0; i < n; i++) 
	{
		char* extensions = (char *)glGetStringi(GL_EXTENSIONS, i);

		if (strstr(extensions, "WGL_EXT_swap_control") != 0)
		{
			flag = true;
			break;
		}
	}

	if (flag == false)
	{
		return;
	}
	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(sync);
	}
}

//init glew & glut
void initGL(int *argc, char **argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitContextVersion(4, 4);
	glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Hello triangle!");

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	setVSync(true);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1.0);

	//glDisable(GL_CULL_FACE);

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);

	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glutReportErrors();
}

void initBlockGroups()
{
	glUseProgram(compute_programme);
	int loc = glGetUniformLocation(compute_programme, "scaleInv");

	VariablePool::lambdax = lambdax;
	VariablePool::lambdaz = lambdaz;
	VariablePool::px = px;
	VariablePool::pz = pz;
	VariablePool::ax = ax;
	VariablePool::az = az;

	VariablePool::cs_ChunkMeshGeneration_ScaleIndex = loc;

	if (useOctree)
	{
		//The octree will be initlized with Update()
		//But the update will be compeleted in another thread now.
		//So this is commanded out.
		if (!useAnotherThreadForMap)
		{
			chunkOctree.Update(cameraPos);
		}
	}
	else
	{
		//LOD Test
		//Nothing here now.
	}
}

//init buffers
void initApp()
{
	useAdvancedRenderingPipeline &= useMeshInsteadOfInstanceCube;
	useOctree &= useMeshInsteadOfInstanceCube & useAdvancedRenderingPipeline;

	//Camera position
	cameraPos.x = -8.456f;
	cameraPos.y = 60.408f;
	cameraPos.z = 14.361f;

	cameraRotX = -0.2f;
	cameraRotY = -2.3f;

	//VAO Creation

	vao_cube = 0;
	glGenVertexArrays(1, &vao_cube);
	glBindVertexArray(vao_cube);

	//VBO Creation

	vbo_position = 0;
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_positions), cube_positions, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	vbo_normal = 0;
	glGenBuffers(1, &vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLint testval = 0;

	/*
	Before using the shaders we have to load the strings into a GL shader, and compile them.
	*/
	if (useAdvancedRenderingPipeline)
	{
		////////////////////////
		//MRT Pass
		////////////////////////
		const char* vertex_shader = filetobuf("simpleVert.vert");
		const char* fragment_shader = filetobuf("simpleFrag_MRT.frag");

		vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vertex_shader, NULL);
		glCompileShader(vs);
		fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fragment_shader, NULL);
		glCompileShader(fs);

		pipeline_MRT = glCreateProgram();
		glAttachShader(pipeline_MRT, fs);
		glAttachShader(pipeline_MRT, vs);
		glLinkProgram(pipeline_MRT);

		CheckShaderCompiling("MRT-Vert", vs);
		CheckShaderCompiling("MRT-Frag", fs);

		CheckProgramLinking("MRT Pass", pipeline_MRT);

		glUseProgram(pipeline_MRT);

		glm::vec4 colorLow = glm::vec4(1.0f, 0.356f, 0.529f, 1.0f);
		glm::vec4 colorHigh = glm::vec4(0.356f, 1.0f, 0.635f, 1.0f);

		glUniform4fv(glGetUniformLocation(pipeline_MRT, "colorLow"), 1, glm::value_ptr(colorLow));
		glUniform4fv(glGetUniformLocation(pipeline_MRT, "colorHigh"), 1, glm::value_ptr(colorHigh));

		////////////////////////
		//AO Pass
		////////////////////////
		free((void *)vertex_shader);
		free((void *)fragment_shader);

		vertex_shader = filetobuf("screenQuad.vert");
		fragment_shader = filetobuf("SSAO.frag");

		vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vertex_shader, NULL);
		glCompileShader(vs);
		fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fragment_shader, NULL);
		glCompileShader(fs);

		pipeline_AO = glCreateProgram();
		glAttachShader(pipeline_AO, fs);
		glAttachShader(pipeline_AO, vs);
		glLinkProgram(pipeline_AO);

		glUseProgram(pipeline_AO);
		glUniform1i(glGetUniformLocation(pipeline_AO, "samplerPosition"), 0);
		glUniform1i(glGetUniformLocation(pipeline_AO, "samplerNormal"), 1);
		glUniform1i(glGetUniformLocation(pipeline_AO, "samplerColor"), 2);

		CheckShaderCompiling("ScrQuad-Vert", vs);
		CheckShaderCompiling("AO-Frag", fs);

		CheckProgramLinking("AO Pass", pipeline_AO);

		printError();
	}
	else
	{
		if (useMeshInsteadOfInstanceCube)
		{
			const char* vertex_shader = filetobuf("simpleVert.vert");
			const char* fragment_shader = filetobuf("simpleFrag.frag");

			vs = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vs, 1, &vertex_shader, NULL);
			glCompileShader(vs);
			fs = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fs, 1, &fragment_shader, NULL);
			glCompileShader(fs);
		}
		else
		{
			const char* vertex_shader = filetobuf("simpleInstance.vert");
			const char* fragment_shader = filetobuf("simpleInstance.frag");

			vs = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vs, 1, &vertex_shader, NULL);
			glCompileShader(vs);
			fs = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fs, 1, &fragment_shader, NULL);
			glCompileShader(fs);
		}

		/*
		Now, these compiled shaders must be combined into a single, executable GPU shader programme.
		We create an empty "program", attach the shaders, then link them together.
		*/
		shader_programme = glCreateProgram();
		glAttachShader(shader_programme, fs);
		glAttachShader(shader_programme, vs);
		glLinkProgram(shader_programme);

		CheckShaderCompiling("Simple-Vert", vs);
		CheckShaderCompiling("Simple-Frag", fs);

		CheckProgramLinking("Simple Pass", shader_programme);

		glUseProgram(shader_programme);

		glm::vec4 colorLow = glm::vec4(1.0f, 0.356f, 0.529f, 1.0f);
		glm::vec4 colorHigh = glm::vec4(0.356f, 1.0f, 0.635f, 1.0f);

		glUniform4fv(glGetUniformLocation(shader_programme, "colorLow"), 1, glm::value_ptr(colorLow));
		glUniform4fv(glGetUniformLocation(shader_programme, "colorHigh"), 1, glm::value_ptr(colorHigh));
	}

	//==Compute shader==

	if (useMeshInsteadOfInstanceCube)
	{
		const char* compute_shader = filetobuf("rawIdToMesh.compute");
		cs = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(cs, 1, &compute_shader, NULL);
		glCompileShader(cs);
	}
	else
	{
		const char* compute_shader = filetobuf("rawIdToCubePos.compute");
		cs = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(cs, 1, &compute_shader, NULL);
		glCompileShader(cs);
	}

	compute_programme = glCreateProgram();
	glAttachShader(compute_programme, cs);
	glLinkProgram(compute_programme);

	glGetShaderiv(cs, GL_COMPILE_STATUS, &testval);
	if (testval == GL_FALSE)
	{
		char infolog[1024];
		glGetShaderInfoLog(cs, 1024, NULL, infolog);
		printf("The compute shader failed to compile with the error: %s \n", infolog);
	}

	///////////////////////////
	//Rendering stuffs
	///////////////////////////

	glGenFramebuffers(1, &frameBuffer_MRT);
	glGenFramebuffers(1, &frameBuffer_SSAO);

	glGenTextures(1, &RT_Position);
	glGenTextures(1, &RT_Normal);
	glGenTextures(1, &RT_Color);
	glGenTextures(1, &RT_AOMap);

	glBindTexture(GL_TEXTURE_2D, RT_Position);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, RT_Normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, RT_Color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, RT_AOMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenRenderbuffers(1, &RT_Depth);
	glBindRenderbuffer(GL_RENDERBUFFER, RT_Depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);

	//MRT Pass

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_MRT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RT_Depth);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, RT_Position, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, RT_Normal, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, RT_Color, 0);
	{
		GLenum DrawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, DrawBuffers); // "1" is the size of DrawBuffers
	}
	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error on FB0 creation\n");
	}

	//SSAO Pass

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_SSAO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, RT_AOMap, 0);
	{
		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
	}
	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error on FB1 creation\n");
	}

	//The screen quad
	ToolBox::printError();
	glGenVertexArrays(1, &vao_screenQuad);
	glBindVertexArray(vao_screenQuad);

	glGenBuffers(1, &vbo_screenQuad);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_screenQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	// Create and compile our GLSL program from the shaders
	//GLuint quad_programID = LoadShaders("Passthrough.vertexshader", "SimpleTexture.fragmentshader");
	//GLuint texID = glGetUniformLocation(quad_programID, "renderedTexture");
	//GLuint timeID = glGetUniformLocation(quad_programID, "time");

	chunkOctree.compute_programme = compute_programme;
	ToolBox::printError();

	ToolBox::LoadMap("height_Snowy.bmp", "color_Snowy.bmp");

	initBlockGroups();
	ToolBox::printError();

	textDrawer.initText2D("./LucidaSansTypewriter.dds");
}

//rendering stuff
void render()
{
	if (useAdvancedRenderingPipeline)
	{
		//MRT Pass
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_MRT);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(pipeline_MRT);

		//camera Pos
		glm::mat4 view = glm::inverse(glm::translate(glm::mat4(1.0f), cameraPos) * cameraRot);
		glm::mat4 proj = glm::perspective(cameraArc, 1.0f * aspectRatio, cameraNear, cameraFar);

		glUniformMatrix4fv(glGetUniformLocation(pipeline_MRT, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(pipeline_MRT, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
		GLint modelUniformIndex = glGetUniformLocation(pipeline_MRT, "model_group");

		printError();

		if (useOctree)
		{
			//chunkOctree.Drawall(36, 2, modelUniformIndex);
			chunkOctree.Drawall_WalkThrough(36, 2, modelUniformIndex);
		}
		else
		{
			for each (auto grp in blockGroupList)
			{
				grp->Draw(36, 2, modelUniformIndex);
			}
		}

		//AO Pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		
		glUseProgram(pipeline_AO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RT_Position);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, RT_Normal);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, RT_Color);

		glBindVertexArray(vao_screenQuad);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Draw some text
		char tmpstr[256];
		
		sprintf(tmpstr, "FPS      : %d", currentFPS);
		textDrawer.printText2D(tmpstr, 0, 900 - 16, 16, 8);

		sprintf(tmpstr, "workList : %d", workListLenth);
		textDrawer.printText2D(tmpstr, 0, 900 - 32, 16, 8);

		sprintf(tmpstr, "Allocated: %d", VariablePool::allocatedGroupCount);
		textDrawer.printText2D(tmpstr, 0, 900 - 48, 16, 8);

		sprintf(tmpstr, "AllocGPU : %d", VariablePool::allocatedGPUGroupCount);
		textDrawer.printText2D(tmpstr, 0, 900 - 64, 16, 8);

		sprintf(tmpstr, "         = %d MBytes", VariablePool::allocatedGPUGroupCount * 3);
		textDrawer.printText2D(tmpstr, 0, 900 - 80, 16, 8);

		int tmp = 0;

		for (int i = 0; i < 6; i++)
		{
			sprintf(tmpstr, "LOD%d     : %d", i, VariablePool::LODCount[i]);
			textDrawer.printText2D(tmpstr, 0, 900 - 96 - (16 * i), 16, 8);

			tmp += VariablePool::LODCount[i];
		}

		sprintf(tmpstr, "LOD Total: %d", tmp);
		textDrawer.printText2D(tmpstr, 0, 900 - 96 - (16 * 6), 16, 8);
	}
	else
	{
		/*
		We draw in a loop. Each iteration draws the screen once;
		a "frame" of rendering. The loop finishes if the window is closed.
		Later we can also ask GLFW is the escape key has been pressed.
		*/

		// wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(shader_programme);

		if (!useMeshInsteadOfInstanceCube)
		{
			glBindVertexArray(vao_cube);
		}

		//camera Pos
		glm::mat4 view = glm::inverse(glm::translate(glm::mat4(1.0f), cameraPos) * cameraRot);
		glm::mat4 proj = glm::perspective(cameraArc, 1.0f * aspectRatio, cameraNear, cameraFar);

		glUniformMatrix4fv(glGetUniformLocation(shader_programme, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader_programme, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
		GLint modelUniformIndex = glGetUniformLocation(shader_programme, "model_group");

		for each (auto grp in blockGroupList)
		{
			grp->Draw(36, 2, modelUniformIndex);
		}
	}

	//put the stuff we've been drawing onto the display
	glutSwapBuffers();
	glutPostRedisplay();
}

//update
void update()
{
	nowTime = ((float)glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
	float dTime = nowTime - prevTime;
	prevTime = nowTime;

	//cameraRot = glm::rotate(cameraRot, -mouse_dx * 180.0f * dTime, glm::vec3(0.0f, 1.0f, 0.0f));
	cameraRotY += -mouse_dx * 2.0f * dTime;
	cameraRotX += -mouse_dy * 2.0f * dTime;
	cameraRot = glm::eulerAngleYX(cameraRotY, cameraRotX);

	mouse_dx = 0;
	mouse_dy = 0;
	
	/*if (cameraRotY < 0)
		cameraRotY += 360.0f;
	else if (cameraRotY > 360.0f)
		cameraRotY -= 360.0f;

	if (cameraRotX < -180)
		cameraRotX = -180.0f;
	else if (cameraRotX > 180.0f)
		cameraRotX = 180.0f;*/

	glm::vec4 camMoveSpeed;

	camMoveSpeed.z += (float)(keyState['s'] - keyState['w']) * scalarSpeed * dTime;
	camMoveSpeed.x += (float)(keyState['d'] - keyState['a']) * scalarSpeed * dTime;
	camMoveSpeed.y += (float)(keyState['q'] - keyState['e']) * scalarSpeed * dTime;
	camMoveSpeed.w = 1.0f;

	cameraPos += glm::vec3(cameraRot * camMoveSpeed);

	if (useOctree)
	{
		int start = glutGet(GLUT_ELAPSED_TIME);
		if (useAnotherThreadForMap)
		{
			glUseProgram(compute_programme);

			//check if there are any work to be done
			int len = chunkOctree.GPUworkList.size();
			workListLenth = len;

			//printf("INFO:\tGPU Work list with Size = %d\n", len);

			//Do the GPU work
			int i;
			for (i = 0; i < len; i++)
			{
				if (i % 8 == 0 && glutGet(GLUT_ELAPSED_TIME) - start > maximumWorkTimeMs)
				{
					break;
				}
				if (chunkOctree.GPUworkList.at(i).isBuild == true)
				{
					chunkOctree.GPUworkList.at(i).node->InitGroupMesh();
					chunkOctree.GPUworkList.at(i).node->BuildGroupMesh();
				}
				else
				{
					if (chunkOctree.GPUworkList.at(i).groupBak != NULL)
					{
						chunkOctree.GPUworkList.at(i).groupBak->FreeBuffers();
						chunkOctree.GPUworkList.at(i).node->group = NULL;
					}
				}
			}

			len = i;
			//Erase the pointers that has already been calculated.
			chunkOctree.GPUworkList.erase(chunkOctree.GPUworkList.begin(), chunkOctree.GPUworkList.begin() + len);

			//Check if there is no works
			if (chunkOctree.GPUworkList.size() <= 0)
			{
				//TODO
				//printf("INFO:\tGPU work done.\n");
				m_condVar.notify_one();
			}
		}
		else
		{
			chunkOctree.Update(cameraPos);
		}
	}

	int start = glutGet(GLUT_ELAPSED_TIME);
	
	//Octree cannot update blockgroups right now.
	if (!useOctree)
	{
		if (updateEveryFrame)
		{
			glUseProgram(compute_programme);

			lambdax = 32.0f + 24.0f * sinf(nowTime * 3.0f);
			lambdaz = 32.0f + 10.0f * sinf(nowTime * 1.0f);

			ax = 10.0f + 6.0f * sinf(nowTime * 4.4f);

#pragma omp parallel for num_threads(16)
			for (int i = 0; i < blockGroupList.size(); i++)
			{
				blockGroupList.at(i)->Init_sinXsinY(
					lambdax, lambdaz, px, pz, ax, az,
					blockGroupList.at(i)->blockGroupPos.x, blockGroupList.at(i)->blockGroupPos.z);
			}
		}
	}

	int dur = glutGet(GLUT_ELAPSED_TIME) - start;
	int loc = glGetUniformLocation(compute_programme, "scaleInv");
	
	for each (auto grp in blockGroupList)
	{
		if (grp->bufferUpdated == false)
		{
			grp->GenerateBuffer(true, loc);
		}
	}

	frameCount++;

	currentFPS = (int)(1.0f / dTime);

	avgFPS += currentFPS;
	if (minFPS > currentFPS)
	{
		minFPS = currentFPS;
	}

	if (frameCount % 60 == 0)
	{
		char windowTitle[50];
		sprintf(windowTitle, "Voxel test - fps: %.2f; updateBlockId: %d ms", 1.0f / dTime, dur);
		printf("avg: %d\tmin: %d\n", avgFPS / frameCount, minFPS);
		glutSetWindowTitle(windowTitle);
	}
}

//Octree update on another thread
void OctreeUpdateMain()
{
	while (!octreeThreadTerminate)
	{
		if (useOctree)
		{
			//chunkOctree.Update(glm::vec3(cameraPos.x, 0, cameraPos.z));
			chunkOctree.Update(cameraPos);
		}
	}
}

//clean up
void cleanup()
{

}

//reshape the viewport
void reshape(int w, int h)
{
	aspectRatio = (float)w / (float)h;
}

//mouse click (state change)?
void mouse(int button, int state, int x, int y)
{
	mousePrevX = x;
	mousePrevY = y;
}

//mouse move (with button clicked)
void motion(int x, int y)
{
	mouse_dx = (float)(x - mousePrevX);
	mouse_dy = (float)(y - mousePrevY);

	mousePrevX = x;
	mousePrevY = y;
}

//keyboard
void key(unsigned char key, int x, int y)
{
	if (key == 'w' || key == 'W')
	{
		keyState['w'] = true;
	}
	if (key == 's' || key == 'S')
	{
		keyState['s'] = true;
	}
	if (key == 'd' || key == 'D')
	{
		keyState['d'] = true;
	}
	if (key == 'a' || key == 'A')
	{
		keyState['a'] = true;
	}
	if (key == 'q' || key == 'Q')
	{
		keyState['q'] = true;
	}
	if (key == 'e' || key == 'E')
	{
		keyState['e'] = true;
	}

	if (key == 'p' || key == 'P')
	{
		updateEveryFrame = !updateEveryFrame;
	}

	if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
	{
		scalarSpeed = spdRun;
	}
	else
	{
		scalarSpeed = spdWalk;
	}
}

void keyUp(unsigned char key, int x, int y)
{
	if (key == 'w' || key == 'W')
	{
		keyState['w'] = false;
	}
	if (key == 's' || key == 'S')
	{
		keyState['s'] = false;
	}
	if (key == 'd' || key == 'D')
	{
		keyState['d'] = false;
	}
	if (key == 'a' || key == 'A')
	{
		keyState['a'] = false;
	}
	if (key == 'q' || key == 'Q')
	{
		keyState['q'] = false;
	}
	if (key == 'e' || key == 'E')
	{
		keyState['e'] = false;
	}
}

//For special keys like F1, arrow keys etc.
void special(int k, int x, int y)
{

}

int main(int argc, char **argv)
{
	initGL(&argc, argv);
	ToolBox::printError();
	initApp();

	std::thread* octreeThread = NULL;

	if (useAnotherThreadForMap)
	{
		octreeThread = new std::thread(OctreeUpdateMain);
	}

	glutDisplayFunc(render);
	glutIdleFunc(update);

	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(key);
	glutKeyboardUpFunc(keyUp);

	glutSpecialFunc(special);

	glutCloseFunc(cleanup);

	glutMainLoop();

	if (useAnotherThreadForMap)
	{
		octreeThreadTerminate = true;
		octreeThread->join();
	}

	return 0;
}