#include<bits/stdc++.h>
#include<GL/glut.h>

using namespace std;

struct Vector3D
{
	double x,y,z;
};

float dot_product(Vector3D a, Vector3D b)
{
    return (a.x*b.x+a.y*b.y+a.z*b.z);
}

Vector3D cross_product(Vector3D a, Vector3D b)
{
    Vector3D p;
    p.x=a.y*b.z-a.z*b.y;
    p.y=a.z*b.x-a.x*b.z;
    p.z=a.x*b.y-a.y*b.x;
    return p;
}


Vector3D product(float a, Vector3D b)
{
    Vector3D p;
    p.x=a*b.x;
    p.y=a*b.y;
    p.z=a*b.z;
    return p;
}

Vector3D divide(float a, Vector3D b)
{
    Vector3D p;
    p.x=b.x*1.0/a;
    p.y=b.y*1.0/a;
    p.z=b.z*1.0/a;
    return p;
}

Vector3D subtract(Vector3D a, Vector3D b)
{
    Vector3D p;
    p.x=a.x-b.x;
    p.y=a.y-b.y;
    p.z=a.z-b.z;
    return p;
}

Vector3D add(Vector3D a, Vector3D b)
{
    Vector3D p;
    p.x=a.x+b.x;
    p.y=a.y+b.y;
    p.z=a.z+b.z;
    return p;
}

Vector3D add(float a, Vector3D b)
{
    Vector3D p;
    p.x=b.x+a;
    p.y=b.y+a;
    p.z=b.z+a;
    return p;
}

Vector3D normalize(Vector3D a)
{
    float ab=dot_product(a, a);
    Vector3D p=divide(sqrt(ab), a);
    return p;
}

class Ray
{
public:
    struct Vector3D start;
    struct Vector3D dir;

    Ray(Vector3D origin, Vector3D direction)
    {
        this->start=origin;
        this->dir=normalize(direction);
    }
};

class Object
{
    public:
        struct Vector3D reference_point;
        double height, width, length;
        double color[3];
        double coEfficients[4]; // ambient, diffuse, specular,reflection coefficients
        int shine; // exponent term of specular component
        Object(){}
        virtual void draw()=0;
        void setColor(double color[])
        {
            for(int i=0; i<3; i++)
                this->color[i]=color[i];
        }
        void setShine(int shine)
        {
            this->shine=shine;
        }
        void setCoEfficients(double coEfficients[])
        {
            for(int i=0; i<4; i++)
                this->coEfficients[i]=coEfficients[i];
        }
        virtual void print() =0;
        virtual double getIntersectPoint()=0
        virtual double intersect(Ray *r, double *color, int level)
        {
            return -1.0;
        }
};

class Sphere: public Object
{
public:
    void drawSphere(double radius, int slices, int stacks)
    {
        struct Vector3D points[60][60];
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
            for(j=0;j<slices;j++)
            {
                glBegin(GL_QUADS);{
                    //upper hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);

                    //lower hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
                }glEnd();
            }
        }
    }
    Sphere(){}
    Sphere(Vector3D center, double radius)
    {
        this->reference_point = center
        this->length = radius
    }
    virtual void draw()
    {
        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        glColor3f(color[0], color[1], color[2]);
        drawSphere(length, 50, 50);
        glPopMatrix();
    }
};

class Floor: public Object
{
public:
    Floor(double floorWidth, double tileWidth)
    {
        this->width=floorWidth;
        this->reference_point={-floorWidth/2,-floorWidth/2, 0};
        this->length=tileWidth;
    }
    void drawBoard()
    {
        for (int i = 0; i < width; i+=length) {
            for (int j = 0; j < width; j+=length) {
                if (i+j % 2 == 0) {
                    glColor3f(1.0, 1.0, 1.0);  // White
                } else {
                    glColor3f(0.0, 0.0, 0.0);  // Black
                }

                glBegin(GL_QUADS);
                glVertex2i(j, i);
                glVertex2i(j + length, i);
                glVertex2i(j + length, i + length);
                glVertex2i(j, i + length);
                glEnd();
            }
        }
    }

    virtual void draw()
    {
        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        drawBoard();
        glPopMatrix();
    }
};

class Triangle: public Object
{
public:
    struct Vector3D corners[3];
    Triangle(){}
    Triangle(Vector3D points[])
    {
        for(int i=0; i<3; i++)
            this->corners[i]=points[i];
    }
    virtual void draw()
    {
        glPushMatrix();
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(corners[0].x, corners[0].y, corners[0].z);
            glVertex3f(corners[1].x, corners[1].y, corners[1].z);
            glVertex3f(corners[2].x, corners[2].y, corners[2].z);
        }
        glEnd();
        glPopMatrix();
    }
};

class Quadratic: public Object
{
public:
    double a, b, c, d, e, f, g, h, i, j;

    virtual void draw()
    {
        return;
    }
};

class PointLight
{
public:
    struct Vector3D light_pos;
    double color[3];
    PointLight(){}
    PointLight(Vector3D light_pos, double color[])
    {
        this->light_pos=light_pos;
        for(int i=0; i<3; i++)
            this->color[i]=color[i];
    }
};

class SpotLight
{
public:
    PointLight point_light;
    struct Vector3D light_direction;
    double cutoff_angle;
    SpotLight(){}
    SpotLight(PointLight point_light, Vector3D light_direction, double cutoff_angle)
    {
        this->point_light=point_light;
        this->light_direction=light_direction;
        this->cutoff_angle=cutoff_angle;
    }
};

int main()
{
    return 0;
}

