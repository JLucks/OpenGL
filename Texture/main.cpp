//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include "Angel.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

int modeCam = 0;
float width = 4.0f, heigth = 3.0f;

GLuint program;
GLuint MatrixID;
GLuint vPosition;
GLuint vColor;
GLuint vao;

glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
glm::mat4 MVP;

glm::vec3 position(3, 2, 2);
glm::vec3 look(0, 0, 0);

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

GLfloat g_RotationSpeed = 0.1f;
GLfloat g_Aspect, g_Angle;

point4 points[NumVertices];
color4 colors[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(-0.5,  0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),
	point4(0.5, -0.5,  0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5,  0.5, -0.5, 1.0),
	point4(0.5,  0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// RGBA olors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void
quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	colorcube();
	g_Angle = 45.0f;
	g_Aspect = width / heigth;
	// Create a vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	program = InitShader("vshader31.glsl", "fshader31.glsl");
	glUseProgram(program);

	MatrixID = glGetUniformLocation(program, "MVP");
	
	// set up vertex arrays
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);


	position = glm::vec3(3, 2, 2);
	look = glm::vec3(0, 0, 0);
	Projection = glm::perspective(g_Angle, g_Aspect, 0.1f, 100.0f);

	View = glm::lookAt(
		position, 
		look, 
		glm::vec3(0, 1, 0)  
	);

	Model = glm::mat4(1.0f);

	MVP = Projection * View * Model;
}

//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);


	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glutSwapBuffers();
}
	
//----------------------------------------------------------------------------

void 
updateVisualization (void)
{
	if (modeCam == 0) {
		position = glm::vec3(3, 2, 2);
		look = glm::vec3(0, 0, 0);
		Projection = glm::perspective(g_Angle, g_Aspect, 0.1f, 100.0f);
	}
	else if (modeCam == 1) {
		position = glm::vec3(0, 0, 2);
		look = glm::vec3(0, 0, 0);
		Projection = glm::ortho(-4.0f, 4.0f, 3.0f, -3.0f, 0.1f, 100.0f);
	}
	else if (modeCam == 2) {
		position = glm::vec3(0, 0, 2);
		look = glm::vec3(-sqrt(2)/2, -sqrt(2)/2, -1.0f);
		Projection = glm::ortho(-4.0f, 4.0f, 3.0f, -3.0f, 0.1f, 100.0f);
	}

	View = glm::lookAt(
		position, 
		look,
		glm::vec3(0, 1, 0)  
	);

	MVP = Projection * View * Model;	
}

//----------------------------------------------------------------------------

void reshape(GLsizei w, GLsizei h)
{
	if (h == 0) h = 1;

	glViewport(0, 0, w, h);

	width = (float) w;
	heigth = (float) h;

	updateVisualization();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:  // Escape key
	case 'q': case 'Q':
		glDeleteBuffers(1, &vPosition);
		glDeleteBuffers(1, &vColor);
		glDeleteProgram(program);
		glDeleteVertexArrays(1, &vao);
		exit(EXIT_SUCCESS);
		break;
	case 'a': case 'A': //Perspectiva
		modeCam = 0;
		break;
	case 's': case 'S': //Orthografica
		modeCam = 1;
		break;
	case 'd': case 'D': //Cavalera
		modeCam = 2;
		break;
	case 'z': case 'Z': //Roll
		Model = glm::rotate(Model, g_RotationSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
		break;
	case 'x': case 'X': //Pitch
		Model = glm::rotate(Model, g_RotationSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case 'c': case 'C': //Yaw
		Model = glm::rotate(Model, g_RotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	}
	updateVisualization();
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Cube");

	glewExperimental = GL_TRUE;
	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);


	glutMainLoop();
	return 0;
}
