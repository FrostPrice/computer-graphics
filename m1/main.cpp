#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <tuple>
#include <math.h>

using vertice = std::tuple<double, double, double>;
using lista_vertices = std::vector<vertice>;
using aresta = std::pair<int, int>;
using lista_arestas = std::vector<aresta>;

struct Poligono
{
	double tamanhoLado;
	int numLados;
	vertice posicao;
	vertice escala;
	double rotacao;
	lista_vertices vertices;
	lista_arestas arestas;
};

Poligono criar_cubo(double cx, double cy, double cz, double lado);
void desenhar(const Poligono &poligono);
void movimentar(Poligono &poligono, double distancia, double angulo);
void escalar(Poligono &poligono, double sx, double sy, double sz = 1.0);
void rotacionar(Poligono &poligono, double angulo);
void display();
void redraw(int value);
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);

Poligono cubo;
int delay = 10;

int main(int argc, char **argv)
{
	cubo = criar_cubo(0, 0, 0, 60); // centralizado

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Cubo 3D - Wireframe");

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard_special);
	glutTimerFunc(delay, redraw, 0);

	glutMainLoop();

	return 0;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(0.0, 0.0, 200.0, // posição da câmera
			  0.0, 0.0, 0.0,   // para onde olha
			  0.0, 1.0, 0.0);  // vetor "para cima"

	desenhar(cubo);

	glutSwapBuffers();
}

void redraw(int value)
{
	glutPostRedisplay();
	glutTimerFunc(delay, redraw, 0);
}

Poligono criar_cubo(double cx, double cy, double cz, double lado)
{
	Poligono cubo;
	cubo.posicao = {cx, cy, cz};
	cubo.tamanhoLado = lado;
	cubo.escala = {1, 1, 1};
	cubo.rotacao = 0;

	double h = lado / 2.0;

	// vértices do cubo
	cubo.vertices = {
		{-h, -h, -h}, {h, -h, -h}, {h, h, -h}, {-h, h, -h}, // base
		{-h, -h, h},
		{h, -h, h},
		{h, h, h},
		{-h, h, h} // topo
	};

	// arestas do cubo
	cubo.arestas = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0}, // base
		{4, 5},
		{5, 6},
		{6, 7},
		{7, 4}, // topo
		{0, 4},
		{1, 5},
		{2, 6},
		{3, 7} // colunas
	};

	// ajustar vértices para a posição
	for (auto &v : cubo.vertices)
	{
		std::get<0>(v) += cx;
		std::get<1>(v) += cy;
		std::get<2>(v) += cz;
	}

	return cubo;
}

void desenhar(const Poligono &poligono)
{
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	for (auto [i, j] : poligono.arestas)
	{
		auto [x1, y1, z1] = poligono.vertices[i];
		auto [x2, y2, z2] = poligono.vertices[j];
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
	}
	glEnd();
}

void movimentar(Poligono &poligono, double distancia, double angulo)
{
	double dx = cos(angulo) * distancia;
	double dy = sin(angulo) * distancia;

	std::get<0>(poligono.posicao) += dx;
	std::get<1>(poligono.posicao) += dy;

	for (auto &v : poligono.vertices)
	{
		std::get<0>(v) += dx;
		std::get<1>(v) += dy;
	}
}

void escalar(Poligono &p, double sx, double sy, double sz)
{
	double cx = std::get<0>(p.posicao);
	double cy = std::get<1>(p.posicao);
	double cz = std::get<2>(p.posicao);

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

void rotacionar(Poligono &p, double angulo)
{
	double cx = std::get<0>(p.posicao);
	double cy = std::get<1>(p.posicao);

	for (auto &v : p.vertices)
	{
		double &x = std::get<0>(v);
		double &y = std::get<1>(v);

		double dx = x - cx;
		double dy = y - cy;

		double x_rot = dx * cos(angulo) - dy * sin(angulo);
		double y_rot = dx * sin(angulo) + dy * cos(angulo);

		x = cx + x_rot;
		y = cy + y_rot;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
	case 'w':
	case 'W':
		escalar(cubo, 1.1, 1.1, 1.1); // aumenta escala
		break;
	case 's':
	case 'S':
		escalar(cubo, 0.9, 0.9, 0.9); // diminui o cubo
		break;
	case 'q':
	case 'Q':
		rotacionar(cubo, -0.1); // rotaciona para esquerda
		break;
	case 'e':
	case 'E':
		rotacionar(cubo, 0.1); // rotaciona para direita
		break;
	case ' ':							// espaço
		cubo = criar_cubo(0, 0, 0, 60); // reset para estado inicial
		break;
	}
}

void keyboard_special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		movimentar(cubo, 10, M_PI / 2);
		break;
	case GLUT_KEY_DOWN:
		movimentar(cubo, 10, 3 * M_PI / 2);
		break;
	case GLUT_KEY_LEFT:
		movimentar(cubo, 10, M_PI);
		break;
	case GLUT_KEY_RIGHT:
		movimentar(cubo, 10, 0);
		break;
	}
}
