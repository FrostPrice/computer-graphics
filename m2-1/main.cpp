#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <GL/freeglut.h>
#include <math.h>
using namespace std;

// Global variables
unsigned int model;
vector<vector<float>> vertices;	  // Vertex positions
vector<vector<float>> normals;	  // Vertex normals
vector<vector<float>> texcoords;  // Texture coordinates (not used in rendering)
vector<vector<int>> faces;		  // Faces as indices into the vertices
vector<vector<int>> face_normals; // Indices of normals for each face

// Transformation and lighting states
float rotY = 0.0f, rotX = 0.0f, rotZ = 0.0f; // Rotation angles
float scale = 1.0f;
float translateX = 0.0f, translateY = 0.0f, translateZ = -105.0f; // Z = Initial camera distance
bool lights[3] = {true, true, true};							  // Toggle for 3 lights
bool lightingFollowsModel = false;								  // false = fixed, true = follows model

// Load a .obj file and parse vertices, normals, and face indices
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
	// Variables to compute bounding box for centering
	float minX = INFINITY, minY = INFINITY, minZ = INFINITY;
	float maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;

	while (getline(file, line))
	{
		istringstream ss(line);
		string type;
		ss >> type;

		// Vertex position
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
		// Vertex normal
		else if (type == "vn")
		{
			float x, y, z;
			ss >> x >> y >> z;
			float len = sqrt(x * x + y * y + z * z); // Normalize the normal
			if (len > 0.0f)
			{
				x /= len;
				y /= len;
				z /= len;
			}
			normals.push_back({x, y, z});
		}
		// Texture coordinate
		else if (type == "vt")
		{
			float u, v;
			ss >> u >> v;
			texcoords.push_back({u, v});
		}
		// Face
		else if (type == "f")
		{
			vector<int> vIndices, nIndices;
			string token;
			while (ss >> token)
			{
				int vi = -1, ni = -1;
				size_t slash1 = token.find('/');
				size_t slash2 = token.find('/', slash1 + 1);

				// Parse face format: v, v/vt, v//vn, or v/vt/vn
				if (slash1 == string::npos)
					vi = stoi(token) - 1;
				else if (slash2 == string::npos)
					vi = stoi(token.substr(0, slash1)) - 1;
				else if (slash2 == slash1 + 1)
				{
					vi = stoi(token.substr(0, slash1)) - 1;
					ni = stoi(token.substr(slash2 + 1)) - 1;
				}
				else
				{
					vi = stoi(token.substr(0, slash1)) - 1;
					ni = stoi(token.substr(slash2 + 1)) - 1;
				}

				vIndices.push_back(vi);
				nIndices.push_back(ni);
			}

			// Convert polygon to triangle fan
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

	// Create OpenGL display list for fast rendering
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

// Set up 3-point lighting
void initLighting()
{
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glDisable(GL_COLOR_MATERIAL); // Disable color-based materials — it will be set manually!

	// Light positions: front, left, and top
	GLfloat light_pos[3][4] = {
		{0.0f, 0.0f, 150.0f, 1.0f},	 // Front (Z+)
		{-150.0f, 0.0f, 0.0f, 1.0f}, // Left (X-)
		{0.0f, 150.0f, 0.0f, 1.0f}	 // Top (Y+)
	};

	// Light 0 (Red) - primarily specular
	GLfloat specular0[] = {1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat ambient0[] = {0.05f, 0.0f, 0.0f, 1.0f}; // Low ambient for red light
	GLfloat diffuse0[] = {0.2f, 0.0f, 0.0f, 1.0f};	// Low diffuse for red light
	// GLfloat ambient0[] = {0.0f, 0.0f, 0.0f, 1.0f}; // No ambient for red light
	// GLfloat diffuse0[] = {0.0f, 0.0f, 0.0f, 1.0f}; // No diffuse for red light

	// Light 1 (Green) - primarily diffuse
	GLfloat diffuse1[] = {0.0f, 1.0f, 0.0f, 1.0f};
	GLfloat ambient1[] = {0.0f, 0.05f, 0.0f, 1.0f}; // Low ambient for green light
	GLfloat specular1[] = {0.0f, 0.2f, 0.0f, 1.0f}; // Low specular for green light
	// GLfloat ambient1[] = {0.0f, 0.0f, 0.0f, 1.0f};	// No ambient for green light
	// GLfloat specular1[] = {0.0f, 0.0f, 0.0f, 1.0f}; // No specular for green light

	// Light 2 (Blue) - primarily ambient
	GLfloat ambient2[] = {0.0f, 0.0f, 1.0f, 1.0f};
	GLfloat diffuse2[] = {0.0f, 0.0f, 0.2f, 1.0f};	// Low diffuse for blue light
	GLfloat specular2[] = {0.0f, 0.0f, 0.2f, 1.0f}; // Low specular for blue light
	// GLfloat diffuse2[] = {0.0f, 0.0f, 0.0f, 1.0f}; // No diffuse for blue light
	// GLfloat specular2[] = {0.0f, 0.0f, 0.0f, 1.0f}; // No specular for blue light

	// Set up each light source
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos[0]);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_POSITION, light_pos[1]);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_POSITION, light_pos[2]);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular2);
	glEnable(GL_LIGHT2);
}

// Render the 3D model
void draw3dObject()
{
	glPushMatrix();
	glTranslatef(translateX, translateY, translateZ);
	glScalef(scale, scale, scale);
	glRotatef(rotX, 1, 0, 0);
	glRotatef(rotY, 0, 1, 0);
	glRotatef(rotZ, 0, 0, 1);
	glCallList(model);
	glPopMatrix();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Reapply lighting positions based on mode
	if (lightingFollowsModel)
	{
		// Position lights *after* transformation so they follow the model
		glPushMatrix();
		glTranslatef(translateX, translateY, translateZ);
		glRotatef(rotX, 1, 0, 0);
		glRotatef(rotY, 0, 1, 0);

		GLfloat light_pos[3][4] = {
			{0.0f, 0.0f, 150.0f, 1.0f},
			{-150.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 150.0f, 0.0f, 1.0f}};
		for (int i = 0; i < 3; ++i)
		{
			if (lights[i])
			{
				glEnable(GL_LIGHT0 + i);
				glLightfv(GL_LIGHT0 + i, GL_POSITION, light_pos[i]);
			}
			else
			{
				glDisable(GL_LIGHT0 + i);
			}
		}
		glPopMatrix();
	}
	else
	{
		// Default: lights stay fixed in world space
		for (int i = 0; i < 3; ++i)
		{
			if (lights[i])
				glEnable(GL_LIGHT0 + i);
			else
				glDisable(GL_LIGHT0 + i);
		}
	}

	// Set manual material properties for the object
	GLfloat mat_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};  // How much ambient light it reflects
	GLfloat mat_diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f};  // Base color under light
	GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Strong specular (shiny white highlights)
	GLfloat mat_shininess = 64.0f;					   // Sharpness of specular reflection

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	glColor3f(0.6f, 0.6f, 0.6f); // Object base color (still useful for color mixing)
	draw3dObject();

	glutSwapBuffers();
}

// Adjust projection on window resize
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

// Redraw periodically
void timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(16, timer, 0);
}

// Handles keyboard input for transforming the model and toggling lights
// CONTROLS:
// 'w', 's' - rotate up/down
// 'a', 'd' - rotate left/right
// 'z', 'x' - rotate counter-clockwise/clockwise around Z-axis
// '+', '-' - zoom in/out
// 'i', 'k' - translate vertically
// 'j', 'l' - translate horizontally
// 'u', 'o' - zoom in/out (move closer/further)
// 'f' - Fix lighting in world space (default)
// 'm' - Make lighting follow model rotation and position
// '1', '2', '3' - toggle lights 0–2 (red, green, blue)
// 'SPACE' - reset all transformations
// 'ESC' - exit program
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		rotY -= 5;
		cout << "Rotated left (Y-axis -5 degrees)" << endl;
		break;
	case 'd':
		rotY += 5;
		cout << "Rotated right (Y-axis +5 degrees)" << endl;
		break;
	case 'w':
		rotX -= 5;
		cout << "Rotated up (X-axis -5 degrees)" << endl;
		break;
	case 's':
		rotX += 5;
		cout << "Rotated down (X-axis +5 degrees)" << endl;
		break;
	case 'z': // Rotate counter-clockwise around Z-axis
		rotZ -= 5;
		cout << "Rotated counter-clockwise (Z-axis -5 degrees)" << endl;
		break;
	case 'x': // Rotate clockwise around Z-axis
		rotZ += 5;
		cout << "Rotated clockwise (Z-axis +5 degrees)" << endl;
		break;
	case '+':
		scale += 0.1f;
		cout << "Scaled up (current scale: " << scale << ")" << endl;
		break;
	case '-':
		scale = max(0.1f, scale - 0.1f);
		cout << "Scaled down (current scale: " << scale << ")" << endl;
		break;
	case 'j':
		translateX -= 2.0f;
		cout << "Translated left (X-axis -2 units)" << endl;
		break;
	case 'l':
		translateX += 2.0f;
		cout << "Translated right (X-axis +2 units)" << endl;
		break;
	case 'i':
		translateY += 2.0f;
		cout << "Translated up (Y-axis +2 units)" << endl;
		break;
	case 'k':
		translateY -= 2.0f;
		cout << "Translated down (Y-axis -2 units)" << endl;
		break;
	case 'u': // Translate closer (zoom in)
		translateZ += 2.0f;
		cout << "Moved closer (Z-axis +2 units)" << endl;
		break;
	case 'o': // Translate further (zoom out)
		translateZ -= 2.0f;
		cout << "Moved further (Z-axis -2 units)" << endl;
		break;
	case 'f':
		lightingFollowsModel = false;
		cout << "Lighting set to fixed (world space)" << endl;
		break;
	case 'm':
		lightingFollowsModel = true;
		cout << "Lighting set to follow model" << endl;
		break;
	case '1':
		lights[0] = !lights[0];
		cout << "Toggled Light 0 (Specular - Red): " << (lights[0] ? "ON" : "OFF") << endl;
		break;
	case '2':
		lights[1] = !lights[1];
		cout << "Toggled Light 1 (Diffuse - Green): " << (lights[1] ? "ON" : "OFF") << endl;
		break;
	case '3':
		lights[2] = !lights[2];
		cout << "Toggled Light 2 (Ambient - Blue): " << (lights[2] ? "ON" : "OFF") << endl;
		break;
	case ' ':
		rotX = rotY = rotZ = 0.0f;
		translateX = translateY = 0.0f;
		translateZ = -105.0f;
		scale = 1.0f;
		cout << "Reset all transformations" << endl;
		break;
	case 27:
		cout << "Exiting program (ESC key)" << endl;
		exit(0);
	}
}

// Mouse state tracking
int lastMouseX, lastMouseY;
bool leftButtonDown = false, rightButtonDown = false;

// Handles mouse button input for rotating/translating the model or zooming
// Left button  - activates rotation when dragging
// Right button - activates translation when dragging
// Scroll up/down - zoom in/out
void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
		leftButtonDown = (state == GLUT_DOWN);
	else if (button == GLUT_RIGHT_BUTTON)
		rightButtonDown = (state == GLUT_DOWN);

	lastMouseX = x;
	lastMouseY = y;

	// Zoom in/out with scroll wheel
	if (button == 3)
		scale += 0.1f;
	else if (button == 4)
		scale = max(0.1f, scale - 0.1f);
}

// Handles mouse motion when a button is held down
// Left drag - rotates the model
// Right drag - translates the model
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

// Entry point
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(900, 600);
	glutCreateWindow("OBJ Loader - 3D Object with Lighting");

	// Register callbacks
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
