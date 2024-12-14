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
double rad;
double direction;
double velocity;
bool dir_change, simulate;
double right, left, up, down;
double camRotate;

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

void drawArrowUp()
{
    glColor3f(0.0, 1.0, 1.0);
    glLineWidth(2.0);
    glBegin(GL_LINES);{
        glVertex3f(0,0,rad);
        glVertex3f(0,0,rad+2);

        glVertex3f(0,0,rad+2);
        glVertex3f(-0.2,0,rad+1.8);

        glVertex3f(0,0,rad+2);
        glVertex3f(0.2,0,rad+1.8);
    }glEnd();
}

void drawDirection()
{
    glColor3f(0.0, 0.0, 1.0);
    glLineWidth(2.0);
    glBegin(GL_LINES);{
        glVertex3f(0,0,0);
        glVertex3f(2+rad,0,0);

        glVertex3f(2+rad,0,0);
        glVertex3f(1.8+rad,0.2,0);

        glVertex3f(2+rad,0,0);
        glVertex3f(1.8+rad,-0.2,0);
    }glEnd();
}

void drawWall()
{
    int x=0, z=0, w=12, h=1;

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex3f(x,0,z);
    glVertex3f(x+w,0,z);
    glVertex3f(x+w,0, z+h);
    glVertex3f(x,0,z+h);
    glEnd();
}

void drawBoundary()
{
    int x[4]={-6, 6, 6, -6};
    int y[4]={-6, -6, 6, 6};
    for(int i=0; i<4; i++)
    {
        glPushMatrix();

        glTranslatef(x[i], y[i], 0);
        glRotatef(i*90, 0,0,1);
        drawWall();
        glPopMatrix();
    }
}

void drawBoard()
{
    int color = 0;
    for (int i = -60; i < 60; i+=2) {
        for (int j = -60; j < 60; j+=2) {
            if (color % 2 == 0) {
                glColor3f(1.0, 1.0, 1.0);  // White
            } else {
                glColor3f(0.0, 0.0, 0.0);  // Black
            }

            glBegin(GL_QUADS);
            glVertex2i(j, i);
            glVertex2i(j + 2, i);
            glVertex2i(j + 2, i + 2);
            glVertex2i(j, i + 2);
            glEnd();

            color++;
        }
        color++;
    }
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

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
			    if(j%2==0)
                    glColor3f(1.0, 0.0, 0.0);
                else
                    glColor3f(0.0, 1.0, 0.0);
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);

				if(j%2==0)
                    glColor3f(0.0, 1.0, 0.0);
                else
                    glColor3f(1.0, 0.0, 0.0);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void collision(point n)
{
    //printf("%f, %f, %f, theta=%f\n", dir.x, dir.y, dir.z, direction);
    dir=subtract(dir, product(2*dot_product(dir, n), n));
    direction = find_angle(dir);
    dir_change=true;
    //printf("%f, %f, %f, %f\n", dir.x, dir.y, dir.z, direction);
}

void keyboardListener(unsigned char key, int x,int y){
    point n;
    double rate =0.1;
	switch (key) {
        case 'i':  // Move forward
            if(!simulate){
                if(dir_change){
                    prev_axis=axis;
                    prev_angle=angle;
                    dir_change=false;
                }
                axis={-sin(direction*pi/180), cos(direction*pi/180), 0};

                angle += velocity * 180.0 / (rad * pi);
                angle=normalize_angle(angle);
                x_pos += velocity * cos(direction * pi / 180.0);
                y_pos += velocity * sin(direction * pi / 180.0);
                if(x_pos+rad>right)
                {
                    x_pos=right-rad;
                    n={-1, 0, 0};
                    collision(n);
                }
                if(x_pos-rad<left){
                    x_pos=left+rad;
                    n={1, 0, 0};
                    collision(n);
                }
                if(y_pos+rad>up){
                    y_pos=up-rad;
                    n={0,-1,0};
                    collision(n);
                }
                if(y_pos-rad<down){
                    y_pos=down+rad;
                    n={0, 1, 0};
                    collision(n);
                }
            }
            break;
        case 'k':  // Move backward
            if(!simulate){
                if(dir_change){
                    prev_axis=axis;
                    prev_angle=angle;
                    dir_change=false;
                }
                axis={-sin(direction*pi/180), cos(direction*pi/180), 0};

                angle -= velocity * 180.0 / (rad * pi);
                angle=normalize_angle(angle);
                x_pos -= velocity * cos(direction* pi / 180.0);
                y_pos -= velocity * sin(direction * pi / 180.0);
                if(x_pos+rad>right)
                {
                    x_pos=right-rad;
                    n={-1, 0, 0};
                    collision(n);
                }
                if(x_pos-rad<left){
                    x_pos=left+rad;
                    n={1, 0, 0};
                    collision(n);
                }
                if(y_pos+rad>up){
                    y_pos=up-rad;
                    n={0,-1,0};
                    collision(n);
                }
                if(y_pos-rad<down){
                    y_pos=down+rad;
                    n={0, 1, 0};
                    collision(n);
                }
            }
            break;
        case 'j':  // Rotate counterclockwise
            direction -= 10.0;
            direction=normalize_angle(direction);
            dir={cos(direction*pi/180), sin(direction*pi/180), 0};
            dir_change=true;
            break;
        case 'l':  // Rotate clockwise
            direction += 10.0;
            direction=normalize_angle(direction);
            dir={cos(direction*pi/180), sin(direction*pi/180), 0};
            dir_change=true;
            break;
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
	glClearColor(1,1,1,0);
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
	//cam_r=cross_product(cam_look, cam_up);

	//gluLookAt(10,10,10,	0,0,0,	0,0,1);
	//gluLookAt(10*cos(cameraAngle), 10*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);
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
    drawBoard();
    drawBoundary();

    glPushMatrix();
    {
        glTranslatef(x_pos,y_pos,rad);
        glRotatef(angle-prev_angle, axis.x, axis.y, axis.z);
        glRotatef(prev_angle, prev_axis.x, prev_axis.y, prev_axis.z);
        drawSphere(rad,8,6);
    }
	glPopMatrix();


	glPushMatrix();
	{
	    glTranslatef(x_pos,y_pos,rad);
	    drawArrowUp();
        glRotatef(direction, 0, 0, 1);
        drawDirection();
	}
    glPopMatrix();



	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void idle()
{
    glutPostRedisplay();
}

void animate(int value){
	//angle+=0.05;
	//codes for any changes in Models, Camera
	if(simulate){
        point n;
        if(dir_change)
        {
            prev_axis=axis;
            prev_angle=angle;
            dir_change=false;
        }
        axis={-sin(direction*pi/180), cos(direction*pi/180), 0};

        angle += velocity * 180.0 * 0.1 / (rad * pi);
        x_pos += velocity * 0.1 * cos(direction* pi / 180.0);
        y_pos += velocity * 0.1 * sin(direction * pi / 180.0);
        if(x_pos+rad>right)
        {
            x_pos=right-rad;
            n={-1, 0, 0};
            collision(n);
        }
        if(x_pos-rad<left){
            x_pos=left+rad;
            n={1, 0, 0};
            collision(n);
        }
        if(y_pos+rad>up){
            y_pos=up-rad;
            n={0,-1,0};
            collision(n);
        }
        if(y_pos-rad<down){
            y_pos=down+rad;
            n={0, 1, 0};
            collision(n);
        }
	}
	glutPostRedisplay();
    glutTimerFunc(100, animate, 0);
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	angle=prev_angle=0;
	x_pos=0;
	y_pos=0;
	direction=45.0;
	rad = 0.5;
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
	cam_pos={10, 10, 10};
	cam_l={-10,-10,-10};
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
	glutTimerFunc(0, animate, 0);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
