#include <bits/stdc++.h>
#include <GL/glut.h>
#include <cmath>
#include <string>
using namespace std;
#include "object.h"
#include "bitmap_image.hpp"

void drawAxes()
{

	glBegin(GL_LINES);
	{
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(300, 0, 0);
		glVertex3f(-300, 0, 0);

		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0, -800, 0);
		glVertex3f(0, 800, 0);

		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0, 0, 300);
		glVertex3f(0, 0, -300);
	}
	glEnd();
}

Vector3D cam_pos, cam_l, cam_r, cam_up;
int imageWidth, imageHeight, windowHeight = 500, windowWidth = 500;
double viewAngle = 80, aspect = 1;
vector<Object *> objects;
vector<PointLight *> pointLights;
vector<SpotLight *> spotLights;
int recursion;
int imageCount = 1;

void capture()
{
	cout << "In capture" << endl;

	bitmap_image image(imageWidth, imageHeight);
	for (int i = 0; i < imageWidth; i++)
	{
		for (int j = 0; j < imageHeight; j++)
		{
			image.set_pixel(i, j, 0, 0, 0);
		}
	}
	double planeDistance = (windowHeight / 2.0) / tan(viewAngle * (pi / 180.0) / 2.0);
	Vector3D topLeft = cam_pos + (cam_l * planeDistance) + (cam_up * (windowHeight * 0.5)) - (cam_r * (windowWidth * 0.5));

	double du = 1.0 * windowWidth / imageWidth;
	double dv = 1.0 * windowHeight / imageHeight;

	topLeft = topLeft + (cam_r * du * 0.5) - (cam_up * dv * 0.5);

	for (int i = 0; i < imageWidth; i++)
	{
		for (int j = 0; j < imageHeight; j++)
		{
		    double nearest=-1, tmin=1e9;

			Vector3D curr = topLeft + (cam_r * du * i) - (cam_up * dv * j);

			Vector3D rayDir = curr - cam_pos;

			Ray *ray = new Ray(cam_pos, rayDir);
			Vector3D color(0, 0, 0);

			for (int k = 0; k < objects.size(); k++)
			{
				Object *o = objects[k];
				Vector3D dummyColor(0, 0, 0);
				double t = o->intersect(ray, dummyColor, 0);
				if (t > 0 && t < tmin)
				{
					tmin = t;
					nearest = k;
				}
			}

			if (nearest != -1)
			{

				double t = objects[nearest]->intersect(ray, color, 1);

				if (color.x > 1)
					color.x = 1;
				if (color.y > 1)
					color.y = 1;
				if (color.z > 1)
					color.z = 1;

				if (color.x < 0)
					color.x = 0;
				if (color.y < 0)
					color.y = 0;
				if (color.z < 0)
					color.z = 0;

				image.set_pixel(i, j, round(color.x * 255), round(color.y * 255), round(color.z * 255));
			}
		}
	}
	string name = "Output_1";
	name += to_string(imageCount);
	name += ".bmp";
	imageCount++;
	image.save_image(name);
	cout << "done" << endl;
}

void keyboardListener(unsigned char key, int x, int y)
{
	double rate = 0.5;
	switch (key)
	{
	case '0':
		capture();
		break;

	case '1':
		cam_r = cam_r * cos(pi / 180) + (cam_up ^ cam_r) * sin(pi / 180);
		cam_l = cam_l * cos(pi / 180) + (cam_up ^ cam_l) * sin(pi / 180);
		break;
	case '2':
		cam_r = cam_r * cos(-pi / 180) + (cam_up ^ cam_r) * sin(-pi / 180);
		cam_l = cam_l * cos(-pi / 180) + (cam_up ^ cam_l) * sin(-pi / 180);
		break;
	case '3':
		cam_l = cam_l * cos(pi / 180) + (cam_r ^ cam_l) * sin(pi / 180);
		cam_up = cam_up * cos(pi / 180) + (cam_r ^ cam_up) * sin(pi / 180);
		break;
	case '4':
		cam_l = cam_l * cos(-pi / 180) + (cam_r ^ cam_l) * sin(-pi / 180);
		cam_up = cam_up * cos(-pi / 180) + (cam_r ^ cam_up) * sin(-pi / 180);
		break;

	case '5':
		cam_up = cam_up * cos(pi / 180) + (cam_l ^ cam_up) * sin(pi / 180);
		cam_r = cam_r * cos(pi / 180) + (cam_l ^ cam_up) * sin(pi / 180);
		break;
	case '6':
		cam_up = cam_up * cos(-pi / 180) + (cam_l ^ cam_up) * sin(-pi / 180);
		cam_r = cam_r * cos(-pi / 180) + (cam_l ^ cam_up) * sin(-pi / 180);
		break;
	default:
		break;
	}
}

void specialKeyListener(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_DOWN: // down arrow key
		// cameraHeight -= 1.0;
		cam_pos = cam_pos - cam_l;
		break;
	case GLUT_KEY_UP: // up arrow key
		// cameraHeight += 1.0;
		cam_pos = cam_pos + cam_l;
		break;

	case GLUT_KEY_RIGHT:
		// cameraAngle += 0.03;
		cam_pos = cam_pos + cam_r;
		break;
	case GLUT_KEY_LEFT:
		// cameraAngle -= 0.03;
		cam_pos = cam_pos - cam_r;
		break;

	case GLUT_KEY_PAGE_UP:
		cam_pos = cam_pos + cam_up;
		break;
	case GLUT_KEY_PAGE_DOWN:
		cam_pos = cam_pos - cam_up;
		break;

	case GLUT_KEY_INSERT:
		break;

	case GLUT_KEY_HOME:
		break;
	case GLUT_KEY_END:
		break;

	default:
		break;
	}
}

void mouseListener(int button, int state, int x, int y)
{ // x, y is the x-y of the screen (2D)
	switch (button)
	{
	case GLUT_LEFT_BUTTON:

		break;

	case GLUT_RIGHT_BUTTON:
		//........
		break;

	case GLUT_MIDDLE_BUTTON:
		//........
		break;

	default:
		break;
	}
}

void display()
{

	// clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0); // color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	// load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	// initialize the matrix
	glLoadIdentity();

	// now give three info
	// 1. where is the camera (viewer)?
	// 2. where is the camera looking?
	// 3. Which direction is the camera's UP direction?

	// gluLookAt(100,100,100,	0,0,0,	0,0,1);
	// gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	// gluLookAt(0,0,200,	0,0,0,	0,1,0);
	gluLookAt(cam_pos.x, cam_pos.y, cam_pos.z,								 // eye position
			  cam_pos.x + cam_l.x, cam_pos.y + cam_l.y, cam_pos.z + cam_l.z, // l=look-eye, look=l+eye
			  cam_up.x, cam_up.y, cam_up.z);								 // up

	// again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);

	/****************************
	/ Add your objects from here
	****************************/
	// add objects

	// glColor3f(1,0,0);
	// drawSquare(10);

	// drawBoard();

	// drawCircle(30,24);

	// drawCone(20,50,24);

	// drawSphere(30,24,20);
	//  glColor3f(0.0, 1.0, 0.0);
	//  glTranslatef(40.0, 0.0, 10.0);
	//  drawSphere(10, 30, 30);

	// glColor3f(1.0, 0.0, 0.0);
	// drawTriangle(a, b, c);
	drawAxes();
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->draw();
	}
	for (int i = 0; i < pointLights.size(); i++)
	{
		pointLights[i]->draw();
	}
	for (int i = 0; i < spotLights.size(); i++)
	{
		spotLights[i]->draw();
	}

	// ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void init()
{
	// codes for initialization

	cam_pos = Vector3D(100.0, 100.0, 10.0);
	cam_l = Vector3D(-1.0 / sqrt(2.0), -1.0 / sqrt(2.0), 0.0);
	cam_up = Vector3D(0.0, 0.0, 1.0);
	cam_r = Vector3D(-1.0 / sqrt(2.0), 1.0 / sqrt(2.0), 0.0);
	// clear the screen
	glClearColor(0, 0, 0, 0);

	/************************
	/ set-up projection here
	************************/
	// load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	// initialize the matrix
	glLoadIdentity();

	// give PERSPECTIVE parameters
	gluPerspective(80, 1, 1, 1000.0);
	// field of view in the Y (vertically)
	// aspect ratio that determines the field of view in the X direction (horizontally)
	// near distance
	// far distance
}

void animate()
{
	// angle+=0.05;
	// codes for any changes in Models, Camera
	glutPostRedisplay();
}

void loadData()
{
	ifstream fin("scene.txt");

	fin >> recursion >> imageHeight;
	imageWidth = imageHeight;

	int n;
	fin >> n;

	for (int i = 0; i < n; i++)
	{
		string shape;
		fin >> shape;

		Object *temp;

		if (shape == "sphere")
		{
			Vector3D center;
			double rad;
			fin >> center.x >> center.y >> center.z;
			fin >> rad;
			temp = new Sphere(center, rad);
		}
		else if (shape == "triangle")
		{
			Vector3D points[3];
			for (int j = 0; j < 3; j++)
			{
				fin >> points[j].x >> points[j].y >> points[j].z;
			}
			temp = new Triangle(points);
		}
		else if (shape == "general")
		{
			temp = new Quadratic();
			Quadratic *temp2 = dynamic_cast<Quadratic *>(temp);
			fin >> temp2->a >> temp2->b >> temp2->c >> temp2->d >> temp2->e >> temp2->f >> temp2->g >> temp2->h >> temp2->i >> temp2->j;
			fin >> temp->reference_point.x >> temp->reference_point.y >> temp->reference_point.z;
			fin >> temp->length >> temp->width >> temp->height;
		}

		fin>>temp->color.x>>temp->color.y>>temp->color.z;
		for (int j = 0; j < 4; j++)
			fin >> temp->coEfficients[j];
		fin >> temp->shine;
		objects.push_back(temp);
	}

	// int m;
	fin >> n;

	for (int i = 0; i < n; i++)
	{
		Vector3D pos;
		Vector3D color;
		fin >> pos.x >> pos.y >> pos.z;
		fin>>color.x>>color.y>>color.z;
		PointLight *pl = new PointLight(pos, color);
		pointLights.push_back(pl);
	}

	fin >> n;

	for (int i = 0; i < n; i++)
	{
		Vector3D pos;
		Vector3D color;
		fin >> pos.x >> pos.y >> pos.z;
		fin>>color.x>>color.y>>color.z;
		PointLight pl(pos, color);
		Vector3D direct;
		double cutoff;
		fin >> direct.x >> direct.y >> direct.z;
		fin >> cutoff;
		SpotLight *sp = new SpotLight(pl, direct, cutoff);
		spotLights.push_back(sp);
	}

	Object *floor = new Floor(1000, 20);
	double floorCoEff[] = {0.5, 0.5, 0.5, 0.5};
	floor->setCoEfficients(floorCoEff);
	floor->setShine(1);
	objects.push_back(floor);
	cout << objects.size() << endl;
}

int main(int argc, char **argv)
{
	loadData();
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST); // enable Depth Testing

	glutDisplayFunc(display); // display callback function
	glutIdleFunc(animate);	  // what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop(); // The main loop of OpenGL

	return 0;
}
