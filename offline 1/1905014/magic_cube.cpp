#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <GL/glut.h>

#define pi (2*acos(0.0))

int drawgrid;
int drawaxes;
double angle, prev_angle;
double x_pos;
double y_pos;
double radius;
double direction;
double velocity;
bool dir_change, simulate;
double right, left, up, down;
double shapeRotate=0;

struct point
{
	double x,y,z;
};

point axis,dir, prev_axis;
point cam_pos, cam_l, cam_r, cam_up;


double normalize_angle(double angle)
{
    while (angle <= -180.0) {
        angle += 360.0;
    }

    while (angle > 180.0) {
        angle -= 360.0;
    }

    return angle;
}

double find_angle(point p)
{
    double ang;
    if(p.x>0 && p.y>0)
    {
        ang=atan(p.y/p.x)*180/pi;
    }
    else if(p.x<0 && p.y>0)
    {
        ang=180.0-atan(p.y/abs(p.x))*180/pi;
    }
    else if(p.x<0 && p.y<0)
    {
        ang=-180.0+atan(abs(p.y)/abs(p.x))*180/pi;
    }
    else if(p.x>0 && p.y<0)
    {
        ang=-atan(abs(p.y)/p.x)*180/pi;
    }
    else if(p.x==0 && p.y>0)
    {
        ang=90.0;
    }
    else if(p.x==0 && p.y<0)
    {
        ang=-90.0;
    }
    else if(p.x>0 && p.y==0)
    {
        ang=0;
    }
    else if(p.x<0 && p.y==0)
    {
        ang=-180;
    }
    return ang;
}

float dot_product(point a, point b)
{
    return (a.x*b.x+a.y*b.y+a.z*b.z);
}

point cross_product(point a, point b)
{
    point p;
    p.x=a.y*b.z-a.z*b.y;
    p.y=a.z*b.x-a.x*b.z;
    p.z=a.x*b.y-a.y*b.x;
    return p;
}


point product(float a, point b)
{
    point p;
    p.x=a*b.x;
    p.y=a*b.y;
    p.z=a*b.z;
    return p;
}

point divide(float a, point b)
{
    point p;
    p.x=b.x*1.0/a;
    p.y=b.y*1.0/a;
    p.z=b.z*1.0/a;
    return p;
}

point subtract(point a, point b)
{
    point p;
    p.x=a.x-b.x;
    p.y=a.y-b.y;
    p.z=a.z-b.z;
    return p;
}

point add(point a, point b)
{
    point p;
    p.x=a.x+b.x;
    p.y=a.y+b.y;
    p.z=a.z+b.z;
    return p;
}

point add(float a, point b)
{
    point p;
    p.x=b.x+a;
    p.y=b.y+a;
    p.z=b.z+a;
    return p;
}

point normalize(point a)
{
    float ab=dot_product(a, a);
    point p=divide(sqrt(ab), a);
    return p;
}


void drawAxes()
{
    glLineWidth(1.0);
	if(drawaxes==1)
	{

		glBegin(GL_LINES);{
		    glColor3f(1.0, 0.0, 0.0);
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}


void drawTriangle()
{
    glBegin(GL_TRIANGLES);
    glVertex3f(1,0,0);
    glVertex3f(0,1,0);
    glVertex3f(0,0,1);
	glEnd();
}

double maxLength=1;
double currLength=1;
double maxRad=maxLength/sqrt(3);
double currRad=0;
double scale=0;

void drawSpiderweb(int numSegments, float radius) {
    glBegin(GL_LINES);

    for (int i = 0; i < numSegments; ++i) {
        float angle = 2.0 * pi * i / numSegments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        glVertex2f(0.0, 0.0);  // Center of the web
        glVertex2f(x, y);      // Points on the circle
    }

    glEnd();

    glBegin(GL_LINE_LOOP);

    for (int i = 0; i < numSegments; ++i) {
        float angle = 2.0 * pi * i / numSegments;
        float x = radius * cosf(angle);
        float y = radius * sinf(angle);

        glVertex2f(x, y);      // Points on the circle
    }

    glEnd();
}

void drawCube()
{
    double change=scale/3;
    for(int i=0; i < 4; i++)
    {
        if(i%2==0)
            glColor3f(1.0f, 0.0f, 1.0f);
        else
            glColor3f(0.0f, 1.0f, 1.0f);
        glPushMatrix();
        {
            glRotatef(i*90.0, 0, 1, 0);
            glTranslatef(change, change, change);
            glScalef(1-scale, 1-scale, 1-scale);
            drawTriangle();
        }
        glPopMatrix();

        if(i%2==0)
            glColor3f(0.0f, 1.0f, 1.0f);
        else
            glColor3f(1.0f, 0.0f, 1.0f);
        glPushMatrix();
        {
            glRotatef(i*90.0, 0, 1, 0);
            glRotatef(180.0, 1, 0, 1);
            glTranslatef(change, change, change);
            glScalef(1-scale, 1-scale, 1-scale);
            drawTriangle();
        }
        glPopMatrix();
    }

}

void drawSphereX(int subdivision)
{
    point n1, n2, v;//y-axis, z-axis, cross
    float y_long, z_long;

    int pointsPerRow=(int)pow(2, subdivision)+1;
    point points[pointsPerRow][pointsPerRow];

    for(int i=0; i<pointsPerRow; i++)
    {
        z_long=pi/180.0*(45.0-90.0*i/(pointsPerRow-1));
        n2.x=-sin(z_long);
        n2.y=cos(z_long);
        n2.z=0;
        for(int j=0; j<pointsPerRow; j++)
        {
            y_long=pi/180.0*(-45.0+90.0*j/(pointsPerRow-1));
            n1.x=-sin(y_long);
            n1.y=0;
            n1.z=-cos(y_long);
            v=normalize(cross_product(n1, n2));
            points[i][j]=product(1.0/sqrt(3), v);
        }
    }
    glBegin(GL_QUADS);
    for(int i = 0; i < pointsPerRow - 1; i++) {
        for(int j = 0; j < pointsPerRow - 1; j++) {
            glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
            glVertex3f(points[i][j+1].x, points[i][j+1].y, points[i][j+1].z);
            glVertex3f(points[i+1][j+1].x, points[i+1][j+1].y, points[i+1][j+1].z);
            glVertex3f(points[i+1][j].x, points[i+1][j].y, points[i+1][j].z);
        }
    }
    glEnd();
}

void drawSphereAll()
{
    for(int i=0;i<2;i++)//red parts
    {
        glPushMatrix();
        {
            glColor3f(1.0f, 0.0f, 0.0f);
            glRotatef(90+180*i,0,0,1);
            glTranslatef(1.0-scale, 0.0, 0.0);
            glScalef(scale, scale, scale);
            drawSphereX(3);
        }
        glPopMatrix();
    }
    for(int i=0;i<2;i++)//green parts
    {
        glPushMatrix();
        {
            glColor3f(0.0f, 1.0f, 0.0f);
            glRotatef(90+180*i,0,1,0);
            glTranslatef(1.0-scale, 0.0, 0.0);
            glScalef(scale, scale, scale);
            drawSphereX(3);
        }
        glPopMatrix();
    }
    for(int i=0;i<2;i++)//blue parts
    {
        glPushMatrix();
        {
            glColor3f(0.0f, 0.0f, 1.0f);
            glRotatef(180*i,0,1,0);
            glTranslatef(1.0-scale, 0.0, 0.0);
            glScalef(scale, scale, scale);
            drawSphereX(3);
        }
        glPopMatrix();
    }
}

void drawCylinderX(double h, double r, int segment)
{
    struct point points[segment+1];
    h *= (1-scale);
    r *= scale;
    glTranslatef((1-scale)/sqrt(2),0,0);
    glRotatef(-70.5287794/2,0,0,1);
    double centerAngle=70.5287794*pi/180.0;
    for (int i = 0; i <= segment; i++)
    {
        double theta = i*centerAngle/segment;
        points[i].x = r*cos(theta);
        points[i].y = r*sin(theta);
    }

    glBegin(GL_QUADS);
        for (int i = 0; i < segment; i++) {
            glVertex3f(points[i].x, points[i].y, h/2);
            glVertex3f(points[i].x, points[i].y, -h/2);
            glVertex3f(points[i+1].x, points[i+1].y, -h/2);
            glVertex3f(points[i+1].x, points[i+1].y, h/2);
        }
    glEnd();
}

void drawCylinderAll()
{
    glColor3f(1.0f, 1.0f, 0.0f);
    for(int i=0;i<4;i++)
    {
        glPushMatrix();
        {
            glRotatef(45+i*90,0,1,0);
            drawCylinderX(sqrt(2),1.0/sqrt(3),36);
        }
        glPopMatrix();
    }

    for(int i=0;i<4;i++)
    {
        glPushMatrix();
        {
            glRotatef(90,1,0,0);
            glRotatef(45+i*90,0,1,0);
            drawCylinderX(sqrt(2),1.0/sqrt(3),36);
        }
        glPopMatrix();
    }

    for(int i=0;i<4;i++)
    {
        glPushMatrix();
        {
            glRotatef(90,0,0,1);
            glRotatef(45+i*90,0,1,0);
            drawCylinderX(sqrt(2),1.0/sqrt(3),36);
        }
        glPopMatrix();
    }
}


void keyboardListener(unsigned char key, int x,int y){
    point n;
    double rate =0.1;
    double sphereChange=maxRad*0.1/maxLength;
	switch (key) {
        case ' ':
            simulate = !simulate;
            break;
        case 'w':
            //cameraHeight += 1.0;
            cam_pos.z += 1.0;
            cam_l.z -= 1.0;
            break;
        case 's':
            //cameraHeight -= 1.0;
            cam_pos.z -= 1.0;
            cam_l.z += 1.0;
            break;
        case 'a':
            shapeRotate += 5.0;
            break;
        case 'd':
            shapeRotate -= 5.0;
            break;
        case '1':
            cam_r = add(product(cos(rate), cam_r), product(sin(rate), cam_l));
            cam_l = subtract(product(cos(rate), cam_l), product(sin(rate), cam_r));
            break;
        case '2':
            cam_r = add(product(cos(-rate), cam_r), product(sin(-rate), cam_l));
            cam_l = subtract(product(cos(-rate), cam_l), product(sin(-rate), cam_r));
            break;
        case '3':
            cam_l = add(product(cos(rate), cam_l), product(sin(rate), cam_up));
            cam_up = subtract(product(cos(rate), cam_up), product(sin(rate), cam_l));
            break;
        case '4':
            cam_l = add(product(cos(-rate), cam_l), product(sin(-rate), cam_up));
            cam_up = subtract(product(cos(-rate), cam_up), product(sin(-rate), cam_l));
            break;

        case '5':
            cam_up = add(product(cos(rate), cam_up), product(sin(rate), cam_r));
            cam_r = subtract(product(cos(rate), cam_r), product(sin(rate), cam_up));
            break;
        case '6':
            cam_up = add(product(cos(-rate), cam_up), product(sin(-rate), cam_r));
            cam_r = subtract(product(cos(-rate), cam_r), product(sin(-rate), cam_up));
            break;
        case ',':
            //currLength-=0.1;
            //currRad+=sphereChange;
            scale+=sphereChange;
            if(scale>=1.0)
            {
                scale=1.0;
            }
            break;
        case '.':
//            currLength+=0.1;
//            currRad-=sphereChange;
            scale-=sphereChange;
            if(scale<=0.0)
            {
                scale=0.0;
            }
            break;
    }
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			//cameraHeight -= 1.0;
			cam_pos=subtract(cam_pos, normalize(cam_l));
			break;
		case GLUT_KEY_UP:		// up arrow key
			//cameraHeight += 1.0;
			cam_pos=add(cam_pos, normalize(cam_l));
			break;

		case GLUT_KEY_RIGHT:
			//cameraAngle += 0.03;
			cam_pos=add(cam_pos, normalize(cam_r));
			break;
		case GLUT_KEY_LEFT:
			//cameraAngle -= 0.03;
			cam_pos=subtract(cam_pos, normalize(cam_r));
			break;

		case GLUT_KEY_PAGE_UP:
		    cam_pos=add(cam_pos, cam_up);
			break;
		case GLUT_KEY_PAGE_DOWN:
		    cam_pos=subtract(cam_pos, cam_up);
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


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
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




void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	cam_r=normalize(cross_product(cam_l, cam_up));
	gluLookAt(cam_pos.x, cam_pos.y, cam_pos.z, //eye position
            cam_pos.x+cam_l.x, cam_pos.y+cam_l.y, cam_pos.z+cam_l.z, //l=look-eye, look=l+eye
            cam_up.x, cam_up.y, cam_up.z);//up
    //gluLookAt(cam_pos.x, cam_pos.y, cam_pos.z, cam_look.x, cam_look.y, cam_look.z, cam_up.x, cam_up.y, cam_up.z);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	glPushMatrix();
	glRotatef(shapeRotate, 0,0 , 1);
	glTranslatef(3*cos(pi*shapeRotate/180),3*sin(pi*shapeRotate/180),0);

    drawCube();
    drawSphereAll();
    drawCylinderAll();
    glPopMatrix();
    //drawSquare(10);
    for(int i=1; i<10; i++)
        drawSpiderweb(16, i);

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void idle()
{
    glutPostRedisplay();
}


void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	angle=prev_angle=0;
	x_pos=0;
	y_pos=0;
	direction=45.0;
	//rad = 0.5;
	axis={0,1,0};
	prev_axis=axis;
	dir={cos(direction*pi/180), sin(direction*pi/180), 0};
	velocity = 0.5;
	dir_change=false;
	right=6;
	left=-6;
	up=6;
	down=-6;
	simulate=false;
	cam_pos={10, 10, 5};
	cam_l={-10,-10,-5};
	cam_up={0,0,1};
	cam_r=cross_product(cam_l, cam_up);

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(60,	1,	1,	50.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(idle);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);
	//glutTimerFunc(0, animate, 0);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
