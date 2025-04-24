#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <GL/freeglut.h>
#include <math.h>
using namespace std;

// Globals
unsigned int model;
vector<vector<float>> vertices;
vector<vector<float>> normals;
vector<vector<float>> texcoords;
vector<vector<int>> faces;
vector<vector<int>> face_normals;

float rotY = 0.0f, rotX = 0.0f;
float scale = 1.0f;
float translateX = 0.0f, translateY = 0.0f;
bool lights[3] = {true, true, true};

void loadObj(string fname)
{
	vertices.clear();
	normals.clear();
	texcoords.clear();
	faces.clear();
	face_normals.clear();

	ifstream file(fname);
	if (!file.is_open())
	{
		cerr << "Failed to open file: " << fname << endl;
		exit(1);
	}

	string line;
	float minX = INFINITY, minY = INFINITY, minZ = INFINITY;
	float maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;

	while (getline(file, line))
	{
		istringstream ss(line);
		string type;
		ss >> type;

		if (type == "v")
		{
			float x, y, z;
			ss >> x >> y >> z;
			vertices.push_back({x, y, z});
			minX = min(minX, x);
			maxX = max(maxX, x);
			minY = min(minY, y);
			maxY = max(maxY, y);
			minZ = min(minZ, z);
			maxZ = max(maxZ, z);
		}
		else if (type == "vn")
		{
			float x, y, z;
			ss >> x >> y >> z;
			// Normalize normal vector
			float len = sqrt(x * x + y * y + z * z);
			if (len > 0.0f)
			{
				x /= len;
				y /= len;
				z /= len;
			}
			normals.push_back({x, y, z});
		}
		else if (type == "vt")
		{
			float u, v;
			ss >> u >> v;
			texcoords.push_back({u, v});
		}
		else if (type == "f")
		{
			vector<int> vIndices, nIndices;
			string token;
			while (ss >> token)
			{
				int vi = -1, ni = -1;
				size_t slash1 = token.find('/');
				size_t slash2 = token.find('/', slash1 + 1);

				if (slash1 == string::npos)
				{
					// f v
					vi = stoi(token) - 1;
				}
				else if (slash2 == string::npos)
				{
					// f v/vt
					vi = stoi(token.substr(0, slash1)) - 1;
				}
				else if (slash2 == slash1 + 1)
				{
					// f v//vn
					vi = stoi(token.substr(0, slash1)) - 1;
					ni = stoi(token.substr(slash2 + 1)) - 1;
				}
				else
				{
					// f v/vt/vn
					vi = stoi(token.substr(0, slash1)) - 1;
					ni = stoi(token.substr(slash2 + 1)) - 1;
				}

				vIndices.push_back(vi);
				nIndices.push_back(ni);
			}

			// Fan triangulation for polygons with > 3 vertices
			for (size_t i = 1; i + 1 < vIndices.size(); ++i)
			{

				faces.push_back({vIndices[0], vIndices[i], vIndices[i + 1]});
				face_normals.push_back({nIndices[0], nIndices[i], nIndices[i + 1]});
			}
		}
	}
	file.close();

	// Center the model
	float centerX = (minX + maxX) / 2.0f;
	float centerY = (minY + maxY) / 2.0f;
	float centerZ = (minZ + maxZ) / 2.0f;
	for (auto &v : vertices)
	{
		v[0] -= centerX;
		v[1] -= centerY;
		v[2] -= centerZ;
	}

	// Generate display list
	model = glGenLists(1);
	glNewList(model, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i < faces.size(); ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int vi = faces[i][j];
			int ni = face_normals[i][j];

			if (ni >= 0 && ni < (int)normals.size())
				glNormal3fv(normals[ni].data());

			if (vi >= 0 && vi < (int)vertices.size())
				glVertex3fv(vertices[vi].data());
			else
				printf("Invalid vertex index: %d\n", vi);
		}
	}
	glEnd();
	glEndList();
}

void initLighting()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);

	GLfloat light_pos[3][4] = {
		{50.0f, 50.0f, 100.0f, 1.0f},
		{-50.0f, 50.0f, 100.0f, 1.0f},
		{0.0f, -50.0f, 100.0f, 1.0f}};

	GLfloat ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat diffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	for (int i = 0; i < 3; ++i)
	{
		GLenum light = GL_LIGHT0 + i;
		glLightfv(light, GL_POSITION, light_pos[i]);
		glLightfv(light, GL_AMBIENT, ambient);
		glLightfv(light, GL_DIFFUSE, diffuse);
		glLightfv(light, GL_SPECULAR, specular);
		glEnable(light);
	}
}

void draw3dObject()
{
	glPushMatrix();
	glTranslatef(translateX, translateY, -105.0f);
	glScalef(scale, scale, scale);
	glRotatef(rotX, 1, 0, 0);
	glRotatef(rotY, 0, 1, 0);
	glCallList(model);
	glPopMatrix();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Toggle lights
	for (int i = 0; i < 3; ++i)
	{
		if (lights[i])
			glEnable(GL_LIGHT0 + i);
		else
			glDisable(GL_LIGHT0 + i);
	}

	glColor3f(0.6, 0.6, 0.6);
	draw3dObject();

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)w / (float)h, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
}

void timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		rotY -= 5;
		break;
	case 'd':
		rotY += 5;
		break;
	case 'w':
		rotX -= 5;
		break;
	case 's':
		rotX += 5;
		break;
	case '+':
		scale += 0.1f;
		break;
	case '-':
		scale = max(0.1f, scale - 0.1f);
		break;
	case 'j':
		translateX -= 2.0f;
		break;
	case 'l':
		translateX += 2.0f;
		break;
	case 'i':
		translateY += 2.0f;
		break;
	case 'k':
		translateY -= 2.0f;
		break;
	case '1':
		lights[0] = !lights[0];
		break;
	case '2':
		lights[1] = !lights[1];
		break;
	case '3':
		lights[2] = !lights[2];
		break;
	case 27:
		exit(0); // ESC
	}
}

int lastMouseX, lastMouseY;
bool leftButtonDown = false, rightButtonDown = false;

void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
		leftButtonDown = (state == GLUT_DOWN);
	else if (button == GLUT_RIGHT_BUTTON)
		rightButtonDown = (state == GLUT_DOWN);

	lastMouseX = x;
	lastMouseY = y;

	// Mouse wheel for zooming
	if (button == 3) // Scroll up
		scale += 0.1f;
	else if (button == 4) // Scroll down
		scale = max(0.1f, scale - 0.1f);
}

void motion(int x, int y)
{
	int dx = x - lastMouseX;
	int dy = y - lastMouseY;

	if (leftButtonDown)
	{
		rotY += dx * 0.5f;
		rotX += dy * 0.5f;
	}
	else if (rightButtonDown)
	{
		translateX += dx * 0.1f;
		translateY -= dy * 0.1f;
	}

	lastMouseX = x;
	lastMouseY = y;
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(900, 600);
	glutCreateWindow("OBJ Loader - 3D Object with Lighting");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseButton);
	glutMotionFunc(motion);
	glutTimerFunc(16, timer, 0);

	initLighting();
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <path_to_obj_file>\n";
		exit(1);
	}
	loadObj(argv[1]);

	glutMainLoop();
	return 0;
}
