#include <bits/stdc++.h>
#include "bitmap_image.hpp"
#include <GL/glut.h>

#define pi (2 * acos(0.0))
#define epsilon 0.00001
using namespace std;

class Vector3D
{
public:
    double x, y, z;
    Vector3D()
    {
        x = 0, y = 0, z = 0;
    }
    Vector3D(double x, double y, double z)
    {
        this->x = x, this->y = y, this->z = z;
    }

    Vector3D operator+(const Vector3D &rhs) const
    {
        return Vector3D(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    Vector3D operator-(Vector3D b)
    {
        return Vector3D(x - b.x, y - b.y, z - b.z);
    }
    Vector3D operator*(double b)
    {
        return Vector3D(x * b, y * b, z * b);
    }
    Vector3D operator/(double b)
    {
        return Vector3D(x / b, y / b, z / b);
    }
    double operator*(Vector3D b)
    {
        return x * b.x + y * b.y + z * b.z;
    }
    Vector3D operator^(Vector3D b)
    {
        return Vector3D(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
    }
    Vector3D operator-()
    {
        return Vector3D(-x, -y, -z);
    }

    double length() { return sqrt(x * x + y * y + z * z); }

    void normalize()
    {
        double len = length();
        x /= len;
        y /= len;
        z /= len;
    }
};

double getAngle(Vector3D a, Vector3D b)
{
    double dot = a * b;
    double mag_a = a.length();
    double mag_b = b.length();

    double cos_theta = dot / (mag_a * mag_b);
    double angle_rad = acos(cos_theta);

    double angle_deg = angle_rad * (180.0 / pi);

    return angle_deg;
}

Vector3D getReflection(Vector3D incidentDir, Vector3D normalDir)
{
    return Vector3D(incidentDir - normalDir * 2 * (incidentDir * normalDir));
}

class Ray
{
public:
    Vector3D start;
    Vector3D dir;

    Ray() {}

    Ray(Vector3D origin, Vector3D direction)
    {
        this->start = origin;
        this->dir = direction;
        this->dir.normalize();
    }
};

class Object;

class PointLight
{
public:
    Vector3D light_pos;
    Vector3D color;
    PointLight() {}
    PointLight(Vector3D light_pos, Vector3D color)
    {
        this->light_pos = light_pos;
        this->color.x = color.x;
        this->color.y = color.y;
        this->color.z = color.z;
    }
    void draw()
    {
        glPushMatrix();
        glPointSize(5);
        glColor3f(color.x, color.y, color.z);
        glBegin(GL_POINTS);
        glVertex3f(light_pos.x, light_pos.y, light_pos.z);
        glEnd();
        glPopMatrix();
    }
};

class SpotLight
{
public:
    PointLight point_light;
    Vector3D light_direction;
    double cutoff_angle;
    SpotLight() {}
    SpotLight(PointLight point_light, Vector3D light_direction, double cutoff_angle)
    {
        this->point_light = point_light;
        this->light_direction = light_direction;
        this->cutoff_angle = cutoff_angle;
    }
    void draw()
    {
        point_light.draw();
    }
};

extern vector<Object *> objects;
extern vector<PointLight *> pointLights;
extern vector<SpotLight *> spotLights;
extern int recursion;

class Object
{
public:
    Vector3D reference_point;
    double height, width, length;
    Vector3D color;
    vector<double> coEfficients = vector<double>(4, 0); // ambient, diffuse, specular,reflection coefficients
    int shine;                                          // exponent term of specular component
    Object() {}
    virtual void draw() = 0;
    void setColor(Vector3D color)
    {
        this->color.x = color.x;
        this->color.y = color.y;
        this->color.z = color.z;
    }
    void setShine(int shine)
    {
        this->shine = shine;
    }
    void setCoEfficients(double coEfficients[])
    {
        for (int i = 0; i < 4; i++)
            this->coEfficients[i] = coEfficients[i];
    }

    virtual Vector3D getColorAt(Vector3D point)
    {
        return Vector3D(this->color.x, this->color.y, this->color.z);
    }

    virtual double findt(Ray *r, Vector3D &color, int level) = 0;

    virtual Ray getNormal(Vector3D point, Ray *incident) = 0;

    virtual string type()
    {
        return "object";
    }

    virtual double intersect(Ray *ray, Vector3D &color, int level)
    {
        double val = findt(ray, color, level);

        if (val < 0)
            return -1;

        if (level == 0)
            return val;

        Vector3D intersectionPoint = ray->start + ray->dir * val;
        Vector3D intersectionPointColor = getColorAt(intersectionPoint);

        color.x = intersectionPointColor.x * coEfficients[0];
        color.y = intersectionPointColor.y * coEfficients[0];
        color.z = intersectionPointColor.z * coEfficients[0];

        for (PointLight *pl : pointLights)
        {
            bool shadow = false;
            Vector3D incidentDir = intersectionPoint - (pl->light_pos);
            incidentDir.normalize();
            Ray *incident = new Ray(pl->light_pos, incidentDir);
            Ray normal = getNormal(intersectionPoint, incident);

            Ray reflect(intersectionPoint, getReflection(incident->dir, normal.dir));

            double t2 = (intersectionPoint - (pl->light_pos)).length();

            if (t2 < 1e-5)
                continue;

            for (Object *o : objects)
            {
                double ot = o->intersect(incident, color, 0);
                if (ot > 0 && ot + 1e-5 < t2)
                {
                    shadow = true;
                    break;
                }
            }

            if (!shadow)
            {
                double lambert = max(0.0, -incidentDir * normal.dir);
                double phong = max(0.0, -ray->dir * reflect.dir);

                color.x = color.x + pl->color.x * coEfficients[1] * lambert *intersectionPointColor.x;
                color.x = color.x + pl->color.x * coEfficients[2] * pow(phong, shine) *intersectionPointColor.x;

                color.y = color.y + pl->color.y * coEfficients[1] * lambert *intersectionPointColor.y;
                color.y = color.y + pl->color.y * coEfficients[2] * pow(phong, shine) *intersectionPointColor.y;

                color.z = color.z + pl->color.z * coEfficients[1] * lambert *intersectionPointColor.z;
                color.z = color.z + pl->color.z * coEfficients[2] * pow(phong, shine) *intersectionPointColor.z;
            }
        }
        for (SpotLight *sp : spotLights)
        {
            bool shadow = false;
            Vector3D incidentDir = intersectionPoint - (sp->point_light.light_pos);
            incidentDir.normalize();
            Ray* incident=new Ray(sp->point_light.light_pos, incidentDir);
            Ray normal = getNormal(intersectionPoint, incident);

            Ray reflect(intersectionPoint, getReflection(incident->dir, normal.dir));

            double angle = getAngle(incident->dir, sp->light_direction);

            if (fabs(angle) > sp->cutoff_angle)
                continue;

            double t2 = (intersectionPoint - (sp->point_light.light_pos)).length();

            if (t2 < 1e-5)
                continue;

            for (Object *o : objects)
            {
                double ot = o->intersect(incident, color, 0);
                if (ot > 0 && ot + 1e-5 < t2)
                {
                    shadow = true;
                    break;
                }
            }

            if (!shadow)
            {
                double lambert = max(0.0, -incidentDir * normal.dir);
                double phong = max(0.0, -ray->dir * reflect.dir);

                color.x = color.x + sp->point_light.color.x * coEfficients[1] * lambert *intersectionPointColor.x;
                color.x = color.x + sp->point_light.color.x * coEfficients[2] * pow(phong, shine) *intersectionPointColor.x;

                color.y = color.y + sp->point_light.color.y * coEfficients[1] * lambert *intersectionPointColor.y;
                color.y = color.y + sp->point_light.color.y * coEfficients[2] * pow(phong, shine) *intersectionPointColor.y;

                color.z = color.z + sp->point_light.color.z * coEfficients[1] * lambert *intersectionPointColor.z;
                color.z = color.z + sp->point_light.color.z * coEfficients[2] * pow(phong, shine) *intersectionPointColor.z;
            }
        }

        // recursive
        if (level < recursion)
        {
            Ray normal = getNormal(intersectionPoint, ray);

            Ray* newReflect=new Ray(intersectionPoint,  getReflection(ray->dir, normal.dir));
            newReflect->start = newReflect->start + newReflect->dir * 1e-5;

            int nearest = -1;
            double tmin = 1e9;

            int k = 0;
            for (Object *o : objects)
            {
                Vector3D dummyColor(0,0,0);
                double tn = o->intersect(newReflect, dummyColor, 0);
                if (tn > 0 && tn < tmin)
                {
                    tmin = tn;
                    nearest = k;
                }
                k++;
            }

            if (nearest != -1)
            {
                Vector3D colRef(0, 0, 0);
                val = objects[nearest]->intersect(newReflect, colRef, level + 1);

                color=color+(colRef*coEfficients[3]);
            }
        }
        return val;
    }

    virtual void print()
    {
        cout << "Object" << endl;
    }
};

class Sphere : public Object
{
public:
    void drawSphere(double radius, int slices, int stacks)
    {
        Vector3D points[60][60];
        int i, j;
        double h, r;
        // generate points
        for (i = 0; i <= stacks; i++)
        {
            h = radius * sin(((double)i / (double)stacks) * (pi / 2));
            r = radius * cos(((double)i / (double)stacks) * (pi / 2));
            for (j = 0; j <= slices; j++)
            {
                points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
                points[i][j].y = r * sin(((double)j / (double)slices) * 2 * pi);
                points[i][j].z = h;
            }
        }
        // draw quads using generated points
        for (i = 0; i < stacks; i++)
        {
            for (j = 0; j < slices; j++)
            {
                glBegin(GL_QUADS);
                {
                    // upper hemisphere
                    glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                    glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                    glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                    glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);

                    // lower hemisphere
                    glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
                    glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
                    glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
                    glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
                }
                glEnd();
            }
        }
    }
    Sphere() {}
    Sphere(Vector3D center, double radius)
    {
        this->reference_point = center;
        this->length = radius;
    }
    virtual void draw()
    {
        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        glColor3f(color.x, color.y, color.z);
        drawSphere(length, 50, 50);
        glPopMatrix();
    }

    virtual double findt(Ray* ray, Vector3D &color, int level)
    {
        Vector3D start = ray->start - reference_point;
        double a = 1, b, c;
        b = 2 * (start * ray->dir);
        c = (start * start) - (length * length);

        double t1, t2;
        double val;
        double det = b * b - 4 * a * c;

        if (det < 0)
            return -1;

        det = sqrt(det);
        t1 = (-b - det) / (2 * a);
        t2 = (-b + det) / (2 * a);
        if (t2 < t1)
            swap(t1, t2);
        if (t1 > 0)
            return t1;
        else if (t2 > 0)
            return t2;
        else
            return -1;
    }

    virtual Ray getNormal(Vector3D point, Ray* incident)
    {
        return Ray(point, point-reference_point);
    }
};

class Floor : public Object
{
public:
    Floor(double floorWidth, double tileWidth)
    {
        this->width = floorWidth;
        this->reference_point = Vector3D(-floorWidth / 2, -floorWidth / 2, 0);
        this->length = tileWidth;
    }

    virtual void draw()
    {
        glPushMatrix();
        for (int i = 0; i < width / length; i++)
        {
            for (int j = 0; j < width / length; j++)
            {
                if (((i + j) % 2) == 0)
                    glColor3f(1, 1, 1);
                else
                    glColor3f(0, 0, 0);
                glBegin(GL_QUADS);
                {
                    glVertex3f(reference_point.x + i * length, reference_point.y + j * length, 0);
                    glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + j * length, 0);
                    glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + (j + 1) * length, 0);
                    glVertex3f(reference_point.x + i * length, reference_point.y + (j + 1) * length, 0);
                }
                glEnd();
            }
        }
        glPopMatrix();
    }

    virtual double findt(Ray* ray, Vector3D &color, int level)
    {
        Vector3D normal = Vector3D(0, 0, 1);
        double dot = normal * ray->dir;

        if (fabs(dot) < 1e-5)
            return -1;

        double t = ((-ray->start)*normal) / dot;

        Vector3D point = ray->start + ray->dir * t;

        if (point.x < reference_point.x || point.x > reference_point.x + width || point.y < reference_point.y || point.y > reference_point.y + width)
            return -1;

        return t;
    }

    virtual Vector3D getColorAt(Vector3D point)
    {

        int i = (point.x - reference_point.x) / length;
        int j = (point.y - reference_point.y) / length;

        if (i < 0 || i >= width / length || j < 0 || j >= width / length)
        {
            return Vector3D(0,0,0);
        }

        if (((i + j) % 2) == 0)
        {
            return Vector3D(1,1,1); // white
        }
        else
        {
            return Vector3D(0,0,0); // black
        }
    }

    virtual Ray getNormal(Vector3D point, Ray* incident)
    {
        Vector3D dir(0,0,1);
        return Ray(point, dir);
    }
};

class Triangle : public Object
{
public:
    Vector3D corners[3];
    Triangle() {}
    Triangle(Vector3D points[])
    {
        for (int i = 0; i < 3; i++)
            this->corners[i] = points[i];
    }
    virtual void draw()
    {
        glPushMatrix();
        glColor3f(color.x, color.y, color.z);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(corners[0].x, corners[0].y, corners[0].z);
            glVertex3f(corners[1].x, corners[1].y, corners[1].z);
            glVertex3f(corners[2].x, corners[2].y, corners[2].z);
        }
        glEnd();
        glPopMatrix();
    }

    virtual Ray getNormal(Vector3D point, Ray* incident)
    {
        Vector3D edge1 = corners[1] - corners[0];
        Vector3D edge2 = corners[2] - corners[0];

        Vector3D normal = edge1 ^ edge2;
        normal.normalize();

        return Ray(point, normal);
    }

    virtual double findt(Ray* ray, Vector3D &color, int level)
    {
        Vector3D edge1=(corners[1]-corners[0]);
        Vector3D edge2=(corners[2]-corners[0]);
        Vector3D rayCrossE2=ray->dir^edge2;
        double det = edge1*rayCrossE2;

        if(det>-epsilon && det<epsilon)//parallel ray
            return -1;

        double inv_det=1.0/det;
        Vector3D s=ray->start-corners[0];
        double u=inv_det*(s*rayCrossE2);

        if(u<0 || u>1)
            return -1;
        Vector3D sCrossE1=s^edge1;
        double v = inv_det*(ray->dir*sCrossE1);

        if(v<0 || u+v>1)
            return -1;

        double t = inv_det*(edge2*sCrossE1);
        return t;
    }
};

class Quadratic : public Object
{
public:
    double a, b, c, d, e, f, g, h, i, j;
    Quadratic() {}

    virtual void draw()
    {
        return;
    }

    bool insideBox(Vector3D point)
    {
        double x = point.x - reference_point.x;
        double y = point.y - reference_point.y;
        double z = point.z - reference_point.z;


        if (fabs(length) > 1e-5)
        {
            if (x < 0 || x > length)
                return false;
        }
        if (fabs(width) > 1e-5)
        {
            if (y < 0 || y > width)
                return false;
        }
        if (fabs(height) > 1e-5)
        {
            if (z < 0 || z > height)
                return false;
        }
        return true;
    }


    virtual Ray getNormal(Vector3D point, Ray* incident)
    {
        double x = point.x, y = point.y, z = point.z;
        Vector3D dir = Vector3D(2 * a * x + d * y + e * z + g, 2 * b * y + d * x + f * z + h, 2 * c * z + e * x + f * y + i);

        return Ray(point, dir);
    }

    virtual double findt(Ray *ray, Vector3D &color, int level)
    {
        double x0 = ray->start.x, y0 = ray->start.y, z0 = ray->start.z;
        double x1 = ray->dir.x, y1 = ray->dir.y, z1 = ray->dir.z;

        double c0 = a * x1 * x1 + b * y1 * y1 + c * z1 * z1 + d * x1 * y1 + e * x1 * z1 + f * y1 * z1;
        double c1 = 2 * a * x0 * x1 + 2 * b * y0 * y1 + 2 * c * z0 * z1 + d * (x0 * y1 + x1 * y0) + e * (x0 * z1 + x1 * z0);
        c1 += f * (y0 * z1 + y1 * z0) + g * x1 + h * y1 + i * z1;
        double c2 = a * x0 * x0 + b * y0 * y0 + c * z0 * z0 + d * x0 * y0 + e * x0 * z0 + f * y0 * z0 + g * x0 + h * y0 + i * z0 + j;
        double t1, t2;

        double det = c1 * c1 - 4 * c0 * c2;
        if (det < 0)
            return -1;
        if (fabs(c0) < 1e-5)
            return -c2 / c1;

        t1 = (-c1 - sqrt(det)) / (2 * c0);
        t2 = (-c1 + sqrt(det)) / (2 * c0);

        if (t1 < 0 && t2 < 0)
            return -1;

        if (t2 < t1)
            swap(t1, t2);

        if (t1 > 0) {
            Vector3D point = ray->start + ray->dir * t1;
            if (insideBox(point)) {
                return t1;
            }
        }
        if (t2 > 0) {
            Vector3D point = ray->start + ray->dir * t2;
            if (insideBox(point)) {
                return t2;
            }
        }
        return -1;
    }
};
