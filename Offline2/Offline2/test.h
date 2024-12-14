#include<bits/stdc++.h>
using namespace std;
using matrix=vector<vector<double>>;

#define pi (2*acos(0.0))

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

void printMatrix(matrix a)
{
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            cout<<a[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
}

h_point multiply(matrix a, h_point b)
{
    double result[4]={0, 0, 0, 0};
    double b_vec[4] = {b.x,b.y,b.z,b.n};
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            result[i]+=a[i][j]*b_vec[j];
        }
        //cout<<result[i]<<endl;
    }
    h_point res(result[0], result[1], result[2], result[3]);
    res=scaleDown(res);
    //printPoint(res);
    return res;
}

matrix multiply(matrix a, matrix b)
{
    matrix result;
    result.resize(4, vector<double>(4, 0));
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            for(int k=0; k<4; k++)
            {
                result[i][j] += a[i][k]*b[k][j];
            }
        }
    }
    return result;
}

matrix identity()
{
    matrix iden;
    iden.resize(4, vector<double>(4, 0));
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            if(i==j)
                iden[i][j]=1;
        }
    }
    return iden;
}

matrix translate(h_point p)
{
    matrix m=identity();
    m[0][3]=p.x;
    m[1][3]=p.y;
    m[2][3]=p.z;
    return m;
}

matrix scale(h_point p)
{
    matrix m=identity();
    m[0][0]=p.x;
    m[1][1]=p.y;
    m[2][2]=p.z;
    //printMatrix(m);
    return m;
}

h_point rodrigues(h_point a, h_point b, double theta)
{
    double rad=theta*pi/180.0;
    h_point result;

    h_point cross=product(sin(rad), cross_product(b, a));
    //cout<<"cross"<<endl;
    //printPoint(cross);
    double dot=dot_product(a, b);
    //cout<<"dot "<<dot<<endl;

    result.x=b.x*cos(rad)+(1-cos(rad))*dot*b.x+cross.x;
    result.y=b.y*cos(rad)+(1-cos(rad))*dot*b.y+cross.y;
    result.z=b.z*cos(rad)+(1-cos(rad))*dot*b.z+cross.z;
    //cout<<"result"<<endl;
    //printPoint(result);
    return result;
}

matrix rotation(double angle, h_point p)
{
    matrix m=identity();

    p = normalize(p);
    //cout<<"rotation point"<<endl;
    //printPoint(p);
    h_point x(1,0,0);
    h_point y(0,1,0);
    h_point z(0,0,1);

    h_point c1 = rodrigues(x,p,angle);
    h_point c2 = rodrigues(y,p,angle);
    h_point c3 = rodrigues(z,p,angle);

    m[0][0] = c1.x;
    m[1][0] = c1.y;
    m[2][0] = c1.z;

    m[0][1] = c2.x;
    m[1][1] = c2.y;
    m[2][1] = c2.z;

    m[0][2] = c3.x;
    m[1][2] = c3.y;
    m[2][2] = c3.z;

    return m;
}

matrix view(h_point eye, h_point look, h_point up)
{
    matrix R=identity();
    h_point l=subtract(look, eye);
    l=normalize(l);
    h_point r=cross_product(l, up);
    r=normalize(r);
    h_point u=cross_product(r, l);
    u=normalize(u);

    R[0][0] = r.x;
    R[0][1] = r.y;
    R[0][2] = r.z;

    R[1][0] = u.x;
    R[1][1] = u.y;
    R[1][2] = u.z;

    R[2][0] = -l.x;
    R[2][1] = -l.y;
    R[2][2] = -l.z;

    matrix T=translate(h_point(-eye.x, -eye.y, -eye.z));
    matrix V=multiply(R, T);
    return V;
}

matrix projection(double fovY, double aspect, double near, double far)
{
    matrix P = identity();
    double fovX = fovY*aspect;
    double t = near*tan((fovY/2)*pi/180.0);
    double r = near*tan((fovX/2)*pi/180.0);

    P[0][0] = near/r;
    P[1][1] = near/t;
    P[2][2] = -(far+near)/(far-near);
    P[3][2] = -1;
    P[2][3] = -2.0*far*near/(far-near);
    P[3][3] = 0;
    return P;
}

//int main()
//{
//    h_point a(2,5,6);
//    h_point x=cross_product(a, a);
//
//    printPoint(x);
//
//    return 0;
//}

