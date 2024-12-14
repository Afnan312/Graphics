#include<bits/stdc++.h>
using namespace std;

struct h_point
{
    double x, y, z, n;
    h_point(double x, double y, double z) : x(x), y(y), z(z) , n(1.0) {}
    h_point(double x, double y, double z, double n) : x(x), y(y), z(z) , n(n) {}
    h_point() : x(0.0), y(0.0), z(0.0), n(1.0) {}
};

double dot_product(h_point a, h_point b)
{
    return (a.x*b.x+a.y*b.y+a.z*b.z);
}

h_point cross_product(h_point a, h_point b)
{
    h_point p;
    p.x=a.y*b.z-a.z*b.y;
    p.y=a.z*b.x-a.x*b.z;
    p.z=a.x*b.y-a.y*b.x;
    return p;
}

h_point product(double a, h_point b)
{
    h_point p;
    p.x=a*b.x;
    p.y=a*b.y;
    p.z=a*b.z;
    return p;
}

h_point divide(double a, h_point b)
{
    h_point p;
    p.x=b.x*1.0/a;
    p.y=b.y*1.0/a;
    p.z=b.z*1.0/a;
    return p;
}

h_point subtract(h_point a, h_point b)
{
    h_point p;
    p.x=a.x-b.x;
    p.y=a.y-b.y;
    p.z=a.z-b.z;
    return p;
}

h_point add(h_point a, h_point b)
{
    h_point p;
    p.x=a.x+b.x;
    p.y=a.y+b.y;
    p.z=a.z+b.z;
    return p;
}

h_point add(float a, h_point b)
{
    h_point p;
    p.x=b.x+a;
    p.y=b.y+a;
    p.z=b.z+a;
    return p;
}

h_point normalize(h_point a)
{
    float ab=dot_product(a, a);
    h_point p=divide(sqrt(ab), a);
    return p;
}

h_point scaleDown(h_point a)
{
    h_point p;
    p.x=a.x/a.n;
    p.y=a.y/a.n;
    p.z=a.z/a.n;
    p.n=a.n/a.n;
    return p;
}

void printPoint(h_point a)
{
    cout<<a.x<<" "<<a.y<<" "<<a.z<<" "<<a.n<<endl;
}

