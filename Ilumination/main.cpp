//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include "Angel.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

float width = 4.0f, heigth = 3.0f;

int lats, longs;
bool isInited;
GLuint m_vao, m_vboVertex, m_vboIndex;
int numsToDraw;

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

GLfloat g_RotationSpeed = 0.1f;
GLfloat g_Aspect, g_Angle;


std::vector<GLfloat> vertices;
std::vector<GLuint> indices;

void
createSphere(void) 
{
	int i, j;
	int indicator = 0;
	for (i = 0; i <= lats; i++) {
		double lat0 = glm::pi<double>() * (-0.5 + (double)(i - 1) / lats);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = glm::pi<double>() * (-0.5 + (double)i / lats);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		for (j = 0; j <= longs; j++) {
			double lng = 2 * glm::pi<double>() * (double)(j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			vertices.push_back(x * zr0);
			vertices.push_back(y * zr0);
			vertices.push_back(z0);
			indices.push_back(indicator);
			indicator++;

			vertices.push_back(x * zr1);
			vertices.push_back(y * zr1);
			vertices.push_back(z1);
			indices.push_back(indicator);
			indicator++;
		}
		indices.push_back(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	}
}


//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{

	isInited = false;
	m_vao = 0;
	m_vboVertex = 0;
	m_vboIndex = 0;

	lats = 40;
	longs = 40;

	g_Angle = 45.0f;
	g_Aspect = width / heigth;

	createSphere;
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
	glutCreateWindow("Iluminate Sphere");

	glewExperimental = GL_TRUE;
	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);


	glutMainLoop();
	return 0;
}
