#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <gl/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <time.h>

#include <iostream>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

typedef struct
{
	int lenth;

	int frameId;
	int width;
	int height;
	int format;
	int dataType;

	float state;
}Header;

const int bufLen = 16384;
const int port = 21563;
const int updateFPS = 60;

int pixelFormat = GL_RGB;
int pixelDataType = GL_UNSIGNED_SHORT_5_6_5;
int pixelSize = 2;

bool asyncStreaming = false;
char* pixelBuffer;

float cameraArc = 1.309f /*75 degrees*/, aspectRatio = 16.0f / 9.0f, cameraNear = 1.0f, cameraFar = 8192.0f;

int mousePrevX = -1, mousePrevY = -1;
float mouse_dx, mouse_dy;

//Rendering stuffs
const int windowHeight = 500, windowWidth = 800;

GLfloat g_quad_vertex_buffer_data[] =
{
	-1.0f, -1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f,
	1.0f,  1.0f, 0.0f, 1.0f,
};

GLuint vao_scrQuad, vbo_scrQuad, vs, fs, shaderProgram;
GLuint pipeline_ScrQuad;

//threads comm
std::mutex mx;
std::condition_variable cv;
std::queue<int> q;
bool finished = false;

float max(float a, float b)
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

//init buffers
void initApp()
{
	pixelBuffer = (char *)malloc(sizeof(char) * windowWidth * windowHeight * pixelSize);

	//Screen Quad
	glGenVertexArrays(1, &vao_scrQuad);
	glBindVertexArray(vao_scrQuad);

	glGenBuffers(1, &vbo_scrQuad);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_scrQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	//Shader
	const char* vertex_shader = filetobuf("scrQuad.vert");
	const char* fragment_shader = filetobuf("sample.frag");

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fs);
	glAttachShader(shaderProgram, vs);
	glLinkProgram(shaderProgram);

	CheckShaderCompiling("Simple-Vert", vs);
	CheckShaderCompiling("Simple-Frag", fs);

	CheckProgramLinking("Simple Pass", shaderProgram);

	glUseProgram(shaderProgram);
}

float currentTime = 0.0f;
float currentState = 0.0f;

//rendering stuff
void render()
{
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);

	currentTime += 0.01f; // frame based instead of time based animation (?)
	currentState = max(0, sin(currentTime));

	glUniform1f(glGetUniformLocation(shaderProgram, "time"), currentTime);
	glUniform2f(glGetUniformLocation(shaderProgram, "resolution"), windowWidth, windowHeight);
	glUniform2f(glGetUniformLocation(shaderProgram, "mouse"), 0, 0);

	glUniform1f(glGetUniformLocation(shaderProgram, "state"), currentState);

	glBindVertexArray(vao_scrQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glutSwapBuffers();
	glutPostRedisplay();
}

//update
void update()
{
	static clock_t lastTime = clock();
	static int frameCount = 0;

	frameCount++;

	if (frameCount % 30 == 0)
	{
		float fps = 30.0f / ((float)(clock() - lastTime) / (float)CLOCKS_PER_SEC);
		printf("FPS = %f\nSpd = %f MBps / s\n\n", 
			fps,
			(float)(windowWidth * windowHeight * pixelSize * fps) / 1024.0f / 128.0f);
		lastTime = clock();
	}

	glReadPixels(0, 0, windowWidth, windowHeight, pixelFormat, pixelDataType, pixelBuffer);

	//std::lock_guard<std::mutex> lk(mx);
	//cv.notify_all();

	//glutTimerFunc(1000 / updateFPS, update, 0);
}

//clean up
void cleanup()
{

}

//reshape the viewport
void reshape(int w, int h)
{
	aspectRatio = (float)w / (float)h;

	free(pixelBuffer);
	pixelBuffer = (char *)malloc(sizeof(char) * windowWidth * windowHeight * pixelSize);
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

}

void keyUp(unsigned char key, int x, int y)
{

}

//For special keys like F1, arrow keys etc.
void special(int k, int x, int y)
{

}

WSADATA wsa;
SOCKET s, socketGraphics, socketControl;
struct sockaddr_in server, client;
int c;
char* message;

void initSock()
{
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		return;
	}

	printf("Initialised.");

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d\n", WSAGetLastError());
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d\n", WSAGetLastError());
	}

	printf("Bind done.\n");

	listen(s, 3);

	printf("Waiting for incoming connections...\n");

	return;
}

bool socketTerminate = false;

void socketMain()
{
	int frame = 0;

	initSock();
	
	c = sizeof(struct sockaddr_in);
	socketGraphics = accept(s, (struct sockaddr *)&client, &c);

	if (socketGraphics == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d\n", WSAGetLastError());
	}

	printf("Connection accepted\n");

	while (!socketTerminate)
	{
		frame++;

		//std::unique_lock<std::mutex> lk(mx);
		//cv.wait(lk);

		//send data
		Header header;

		header.lenth = sizeof(char) * windowWidth * windowHeight * pixelSize;

		header.frameId = frame;
		header.format = pixelFormat;
		header.dataType = pixelDataType;
		header.width = windowWidth;
		header.height = windowHeight;

		header.state = currentState;

		send(socketGraphics, (const char*)&header, sizeof(Header), 0);

		int len;

		for (len = 0; len < header.lenth;)
		{
			len += send(socketGraphics, (const char*)(pixelBuffer + len), header.lenth - len, 0);
		}

		//force sync
		char tmp;
		recv(socketGraphics, &tmp, sizeof(tmp), 0);
	}
}

int main(int argc, char **argv)
{
	std::thread* socketThread = new std::thread(socketMain);

	initGL(&argc, argv);
	printError();
	initApp();

	glutDisplayFunc(render);
	//glutTimerFunc(1000 / updateFPS, update, 0);
	glutIdleFunc(update);

	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(key);
	glutKeyboardUpFunc(keyUp);

	glutSpecialFunc(special);

	glutCloseFunc(cleanup);

	glutMainLoop();

	socketTerminate = true;
	socketThread->join();

	return 0;
}
