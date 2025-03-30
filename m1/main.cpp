#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <tuple>
#include <math.h>

using vertex = std::tuple<double, double, double>;
using vertex_list = std::vector<vertex>;
using edge = std::pair<int, int>;
using edge_list = std::vector<edge>;

struct Polygon3D
{
	double sideLength;
	vertex position;
	vertex scale;
	double rotX, rotY, rotZ;
	vertex_list vertices;
	edge_list edges;
};

Polygon3D create_cube(double cx, double cy, double cz, double side);
void draw(const Polygon3D &polygon);
void translate(Polygon3D &polygon, double distance, double angle, double dz);
void scale_polygon(Polygon3D &polygon, double sx, double sy, double sz = 1.0);
void rotate(Polygon3D &polygon, double angle, char axis);
void display();
void redraw(int value);
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);

Polygon3D cube;
int delay = 10;

int main(int argc, char **argv)
{
	cube = create_cube(0, 0, 0, 60); // centered

	GLsizei height = 600;
	GLsizei width = 600;
	GLfloat aspect = (GLfloat)height / (GLfloat)width; // aspect ratio, so that the image is not distorted

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("3D Cube - Wireframe");

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST); // Enable depth test

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, aspect, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard_special);
	glutTimerFunc(delay, redraw, 0);

	glutMainLoop();
	return 0;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(0.0, 0.0, 200.0, // camera position
			  0.0, 0.0, 0.0,   // look at point
			  0.0, 1.0, 0.0);  // up vector

	draw(cube);
	glutSwapBuffers();
}

void redraw(int value)
{
	GLsizei height = 600;
	GLsizei width = 600;
	if (height == 0)
		height = 1;

	GLfloat aspect = (GLfloat)width / (GLfloat)height;
	glViewport(0, 0, width, height);
	gluPerspective(45.0, aspect, 1.0, 500.0);

	glutPostRedisplay();
	glutTimerFunc(delay, redraw, 0);
}

Polygon3D create_cube(double cx, double cy, double cz, double side)
{
	Polygon3D cube;
	cube.position = {cx, cy, cz};
	cube.sideLength = side;
	cube.scale = {1, 1, 1};
	cube.rotX = cube.rotY = cube.rotZ = 0;

	double h = side / 2.0;

	cube.vertices = {
		{-h, -h, -h}, {h, -h, -h}, {h, h, -h}, {-h, h, -h}, {-h, -h, h}, {h, -h, h}, {h, h, h}, {-h, h, h}};

	cube.edges = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

	for (auto &v : cube.vertices)
	{
		std::get<0>(v) += cx;
		std::get<1>(v) += cy;
		std::get<2>(v) += cz;
	}

	return cube;
}

void draw(const Polygon3D &polygon)
{
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	for (auto [i, j] : polygon.edges)
	{
		auto [x1, y1, z1] = polygon.vertices[i];
		auto [x2, y2, z2] = polygon.vertices[j];
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
	}
	glEnd();
}

void translate(Polygon3D &polygon, double distance, double angle, double dz)
{
	double dx = cos(angle) * distance;
	double dy = sin(angle) * distance;

	std::get<0>(polygon.position) += dx;
	std::get<1>(polygon.position) += dy;
	std::get<2>(polygon.position) += dz;

	for (auto &v : polygon.vertices)
	{
		std::get<0>(v) += dx;
		std::get<1>(v) += dy;
		std::get<2>(v) += dz;
	}
}

void scale_polygon(Polygon3D &p, double sx, double sy, double sz)
{
	double cx = std::get<0>(p.position);
	double cy = std::get<1>(p.position);
	double cz = std::get<2>(p.position);

	for (auto &v : p.vertices)
	{
		auto &x = std::get<0>(v);
		auto &y = std::get<1>(v);
		auto &z = std::get<2>(v);

		x = cx + (x - cx) * sx;
		y = cy + (y - cy) * sy;
		z = cz + (z - cz) * sz;
	}
}

void rotate(Polygon3D &p, double angle, char axis)
{
	double cx = std::get<0>(p.position);
	double cy = std::get<1>(p.position);
	double cz = std::get<2>(p.position);

	for (auto &v : p.vertices)
	{
		double &x = std::get<0>(v);
		double &y = std::get<1>(v);
		double &z = std::get<2>(v);

		double dx = x - cx;
		double dy = y - cy;
		double dz = z - cz;

		if (axis == 'x')
		{
			double y_rot = dy * cos(angle) - dz * sin(angle);
			double z_rot = dy * sin(angle) + dz * cos(angle);
			y = cy + y_rot;
			z = cz + z_rot;
		}
		else if (axis == 'y')
		{
			double x_rot = dx * cos(angle) + dz * sin(angle);
			double z_rot = -dx * sin(angle) + dz * cos(angle);
			x = cx + x_rot;
			z = cz + z_rot;
		}
		else if (axis == 'z')
		{
			double x_rot = dx * cos(angle) - dy * sin(angle);
			double y_rot = dx * sin(angle) + dy * cos(angle);
			x = cx + x_rot;
			y = cy + y_rot;
		}
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // ESC key – exit the program
		exit(0);

	case 'w': // Rotate around X-axis (positive direction)
		rotate(cube, 0.1, 'x');
		break;

	case 's': // Rotate around X-axis (negative direction)
		rotate(cube, -0.1, 'x');
		break;

	case 'a': // Rotate around Y-axis (positive direction)
		rotate(cube, 0.1, 'y');
		break;

	case 'd': // Rotate around Y-axis (negative direction)
		rotate(cube, -0.1, 'y');
		break;

	case 'q': // Rotate around Z-axis (positive direction)
		rotate(cube, 0.1, 'z');
		break;

	case 'e': // Rotate around Z-axis (negative direction)
		rotate(cube, -0.1, 'z');
		break;

	case 'z': // Move into the screen (negative Z)
		translate(cube, 0, 0, -10);
		break;

	case 'x': // Move out of the screen (positive Z)
		translate(cube, 0, 0, 10);
		break;

	case '+':
	case '=': // Scale up the cube by 10%
		scale_polygon(cube, 1.1, 1.1, 1.1);
		break;

	case '-':
	case '_': // Scale down the cube by 10%
		scale_polygon(cube, 0.9, 0.9, 0.9);
		break;

	case ' ': // Spacebar – reset the cube to original state
		cube = create_cube(0, 0, 0, 60);
		break;
	}
}

void keyboard_special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP: // Move upward along Y-axis
		translate(cube, 10, M_PI / 2, 0);
		break;

	case GLUT_KEY_DOWN: // Move downward along Y-axis
		translate(cube, 10, 3 * M_PI / 2, 0);
		break;

	case GLUT_KEY_LEFT: // Move left along X-axis
		translate(cube, 10, M_PI, 0);
		break;

	case GLUT_KEY_RIGHT: // Move right along X-axis
		translate(cube, 10, 0, 0);
		break;
	}
}
