#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <gl/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "blockGroup.h"

float points[] =
{
	0.0f,  0.5f,  0.0f,
	0.5f, -0.5f,  0.0f,
	-0.5f, -0.5f,  0.0f
};
GLuint vao, vbo, vs, fs, cs, shader_programme, compute_programme;

std::vector<blockGroup> blockGroupList;

int groupCountX = 5, groupCountZ = 5;
float fx = 1, fy = 2, ax = 16, ay = 22, px = 3, py = 2;

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

//init glew & glut
void initGL(int *argc, char **argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Hello triangle!");

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.25, 0.25, 0.25, 1.0);

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);

	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glutReportErrors();
}

void initBlockGroups()
{
	for (int x = 0; x < groupCountX; x++)
	{
		for (int z = 0; z < groupCountZ; z++)
		{
			blockGroup grp;
			grp.Init_sinXsinY(fx, fy, px, py, ax, ay, x * 32.0f, z * 32.0f);
			grp.InitBuffers(compute_programme);
			grp.GenerateBuffer();
		}
	}
}

//init buffers
void initApp()
{
	/*
	We will copy this chunk of memory onto the graphics card in a unit called a vertex buffer object (VBO).
	To do this we "generate" an empty buffer, set it as the current buffer in OpenGL's state machine by "binding",
	then copy the points into the currently bound buffer:
	*/

	vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

	/*
	Now an unusual step.
	Most meshes will use a collection of one or more vertex buffer objects to hold vertex points,
	texture-coordinates, vertex normals, etc. In older GL implementations we would have to bind each one,
	and define their memory layout, every time that we draw the mesh.

	To simplify that, we have new thing called the vertex array object (VAO),
	which remembers all of the vertex buffers that you want to use, and the memory layout of each one.

	We set up the vertex array object once per mesh.
	When we want to draw, all we do then is bind the VAO and draw.
	*/

	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	/*
	Before using the shaders we have to load the strings into a GL shader, and compile them.
	*/

	const char* vertex_shader = filetobuf("simple.vert");
	const char* fragment_shader = filetobuf("simple.frag");

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	/*
	Now, these compiled shaders must be combined into a single, executable GPU shader programme.
	We create an empty "program", attach the shaders, then link them together.
	*/
	shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);

	//==Compute shader==

	const char* compute_shader = filetobuf("rawIdToCubePos.compute");
	cs = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(cs, 1, &compute_shader, NULL);
	glCompileShader(cs);

	compute_programme = glCreateProgram();
	glAttachShader(compute_programme, cs);
	glLinkProgram(compute_programme);

	initBlockGroups();
}

//rendering stuff
void render()
{
	/*
	We draw in a loop. Each iteration draws the screen once;
	a "frame" of rendering. The loop finishes if the window is closed.
	Later we can also ask GLFW is the escape key has been pressed.
	*/

	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader_programme);
	glBindVertexArray(vao);
	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawArrays(GL_TRIANGLES, 0, 3);
	// put the stuff we've been drawing onto the display
	glutSwapBuffers();
}

//update
void update()
{
	for each (auto grp in blockGroupList)
	{
		if (grp.bufferUpdated == false)
		{
			grp.GenerateBuffer();
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

}

//mouse click (state change)?
void mouse(int button, int state, int x, int y)
{

}

//mouse move (with button clicked)
void motion(int x, int y)
{

}

//keyboard
void key(unsigned char key, int x, int y)
{

}

//For special keys like F1, arrow keys etc.
void special(int k, int x, int y)
{

}

int main(int argc, char **argv)
{
	initGL(&argc, argv);
	initApp();

	glutDisplayFunc(render);
	glutIdleFunc(update);

	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(key);
	glutSpecialFunc(special);

	glutCloseFunc(cleanup);

	glutMainLoop();

	return 0;
}