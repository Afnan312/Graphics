#include "point.h"
#include "matrix.h"
#include "bitmap_image.hpp"
#include<math.h>

#define eps 0.0000001

static unsigned long int g_seed = 1;
inline int random()
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

struct triangle
{
    h_point points[3];
    int rgb[3];

    triangle()
    {
        rgb[0] = random()%256;
        rgb[1] = random()%256;
        rgb[2] = random()%256;
    }

    triangle(h_point a, h_point b, h_point c)
    {
        points[0]=a;
        points[1]=b;
        points[2]=c;
        rgb[0] = random()%256;
        rgb[1] = random()%256;
        rgb[2] = random()%256;
    }
};

double findX(h_point a, h_point b, double val)
{
    double x_val;

    x_val = a.x - (a.x - b.x)*(a.y - val)/(a.y - b.y);
    return x_val;
}

double findZ(h_point a, h_point b, double val)
{
    double z_val;
    z_val = a.z - (a.z - b.z)*(a.y - val)/(a.y - b.y);
    return z_val;
}

int main()
{
    h_point eye,look,up;

    ifstream fin("scene.txt");
    ofstream fout("stage1.txt");

    fin >> eye.x >> eye.y >> eye.z;
    fin >> look.x >> look.y >> look.z;
    fin >> up.x >> up.y >> up.z;

    double fovY, aspectRatio, near, far;
    fin >> fovY >> aspectRatio >> near >> far;

    stack<matrix> stck;
    matrix m=identity();

    int num_triangles = 0;

    while(true)
    {
        string cmd;
        fin>>cmd;

        if(cmd == "triangle")
        {
            h_point p1,p2,p3;
            fin>>p1.x>>p1.y>>p1.z;
            fin>>p2.x>>p2.y>>p2.z;
            fin>>p3.x>>p3.y>>p3.z;

//            printPoint(p1);
//            printPoint(p2);
//            printPoint(p3);
//
//            cout<<endl;
//            printMatrix(m);

            p1=multiply(m, p1);
            p2=multiply(m, p2);
            p3=multiply(m, p3);

            fout<<fixed<<setprecision(7)<<p1.x<<" "<<p1.y<<" "<<p1.z<<endl;
            fout<<p2.x<<" "<<p2.y<<" "<<p2.z<<endl;
            fout<<p3.x<<" "<<p3.y<<" "<<p3.z<<endl;
            fout<<endl;

            num_triangles++;
        }
        else if(cmd == "translate")
        {
            h_point p;
            fin>>p.x >> p.y >> p.z;

            matrix T=translate(p);
            m=multiply(m, T);
        }
        else if(cmd == "scale")
        {
            h_point p;
            fin>>p.x>>p.y>>p.z;

            matrix T=scale(p);
            m=multiply(m, T);
        }
        else if(cmd == "rotate")
        {
            double angle;
            h_point p;
            fin>>angle>>p.x>>p.y>>p.z;

            matrix T=rotation(angle, p);
            m=multiply(m, T);
        }
        else if(cmd == "push")
        {
            stck.push(m);
        }
        else if(cmd == "pop")
        {
            m=stck.top();
            stck.pop();
        }
        else if(cmd == "end")
        {
            break;
        }
    }

    fin.close();
    fout.close();

    fin.open("stage1.txt");
    fout.open("stage2.txt");

    matrix V=view(eye, look, up);

    for(int i=0; i<num_triangles; i++)
    {
        h_point p1,p2,p3;
        fin>>p1.x>>p1.y>>p1.z;
        fin>>p2.x>>p2.y>>p2.z;
        fin>>p3.x>>p3.y>>p3.z;

        p1 = multiply(V, p1);
        p2 = multiply(V, p2);
        p3 = multiply(V, p3);

        fout<<fixed<<setprecision(7)<<p1.x<<" "<<p1.y<<" "<<p1.z<<endl;
        fout<<p2.x<<" "<<p2.y<<" "<<p2.z<<endl;
        fout<<p3.x<<" "<<p3.y<<" "<<p3.z<<endl;
        fout<<endl;
    }

    fin.close();
    fout.close();

    fin.open("stage2.txt");
    fout.open("stage3.txt");

    matrix P=projection(fovY, aspectRatio, near, far);

    for(int i=0; i<num_triangles; i++)
    {
        h_point p1,p2,p3;
        fin>>p1.x>>p1.y>>p1.z;
        fin>>p2.x>>p2.y>>p2.z;
        fin>>p3.x>>p3.y>>p3.z;

        p1 = multiply(P, p1);
        p2 = multiply(P, p2);
        p3 = multiply(P, p3);

        fout<<fixed<<setprecision(7)<<p1.x<<" "<<p1.y<<" "<<p1.z<<endl;
        fout<<p2.x<<" "<<p2.y<<" "<<p2.z<<endl;
        fout<<p3.x<<" "<<p3.y<<" "<<p3.z<<endl;
        fout<<endl;
    }

    fin.close();
    fout.close();

    fin.open("config.txt");
    fout.open("z_buffer.txt");

    int screenWidth, screenHeight;

    ///////////////////////////////////read data start
    vector<triangle> triangles;
    fin>>screenWidth>>screenHeight;
    //cout<<screenWidth<<endl;
    //cout<<screenHeight<<endl;

    fin.close();
    fin.open("stage3.txt");
    for(int i=0; i<num_triangles; i++)
    {
        h_point p1,p2,p3;
        fin>>p1.x>>p1.y>>p1.z;
        fin>>p2.x>>p2.y>>p2.z;
        fin>>p3.x>>p3.y>>p3.z;


        triangle t(p1, p2, p3);
        triangles.push_back(t);
    }

    ////read data end

    ///////////////////////////////////////////init z_buffer and frame start
    int rightLimit=1;
    int leftLimit=-1;
    int topLimit=1;
    int bottomLimit=-1;

    double dx=(rightLimit-leftLimit)*1.0/screenWidth;
    double dy=(topLimit-bottomLimit)*1.0/screenHeight;

//    Top_Y = top limit along Y-axis - dy/2
//    Left_X = left limit along X-axis + dx/2

    double topY = topLimit-dy/2;
    double leftX = leftLimit+dx/2;
    double bottomY = bottomLimit+dy/2;
    double rightX = rightLimit-dx/2;

    double z_max = 1.0;
    double z_front_limit=-1.0;

    matrix z_buffer;
    z_buffer.resize(screenWidth, vector<double>(screenHeight, z_max));
    bitmap_image* img=new bitmap_image(screenWidth, screenHeight);

    for(int i=0;i<screenWidth;i++)
    {
        for(int j=0;j<screenHeight;j++)
        {
            img->set_pixel(i,j,0,0,0);
        }
    }
    /////init z_buffer and frame end

    //////////////////////////////////////////procedure
    for(int i=0; i<num_triangles; i++)//iterate over the triangles
    {
        triangle t=triangles[i];
        h_point a=t.points[0];
        h_point b=t.points[1];
        h_point c=t.points[2];

        double max_y=max(max(a.y, b.y), c.y);
        double min_y=min(min(a.y, b.y), c.y);
        double max_x=max(max(a.x, b.x), c.x);
        double min_x=min(min(a.x, b.x), c.x);

        //remove triangles outside the boundary
        if(min_y>topY)
            continue;
        else if(max_y<bottomY)
            continue;
        else if(min_x>rightX)
            continue;
        else if(max_x<leftX)
            continue;

        //find min and max dimensions for clipping
        if(max_y>topY)
            max_y=topY;

        if(min_y<bottomY)
            min_y=bottomY;

        if(max_x>rightX)
            max_x=rightX;

        if(min_x<leftX)
            min_x=leftX;

        int row_start = ceil((topY - max_y)/dy);////row start from the top
        int row_end = int((topY - min_y)/dy);////row end in the bottom

        for(int row_no=row_start; row_no<=row_end; row_no++)////iterate starting from the topmost row containing the triangle
        {
            double val=topY-row_no*dy;////y value of the row

            vector<double> x_ab(2), z_ab(2), prev_x(2);/////contains the left and right intersecting x, z co-ordinates of the triangle at that row
            int cnt=0;

            if(abs(a.y-b.y)>eps && val>=min(a.y, b.y) && val<=max(a.y, b.y))
            {
                x_ab[cnt]=findX(a, b, val);
                z_ab[cnt]=findZ(a, b, val);
                cnt++;
            }

            if(abs(b.y-c.y)>eps && val>=min(b.y, c.y) && val<=max(b.y, c.y))
            {
                x_ab[cnt]=findX(b, c, val);
                z_ab[cnt]=findZ(b, c, val);
                cnt++;
            }

            if(abs(c.y-a.y)>eps && val>=min(c.y, a.y) && val<=max(c.y, a.y))
            {
                x_ab[cnt]=findX(c, a, val);
                z_ab[cnt]=findZ(c, a, val);
                cnt++;
            }

            //cout<<cnt<<endl;

            if(x_ab[0]>=x_ab[1])////////if xa is not at left side, swap all values of the vectors
            {
                swap(x_ab[0], x_ab[1]);
                swap(z_ab[0], z_ab[1]);
            }

            prev_x = x_ab;
            bool clipped=false;

            ///////clip the x-values within the bounds
            if(x_ab[0]<min_x)
            {
                x_ab[0]=min_x;
                clipped=true;
            }
            else if(x_ab[0]>max_x)
            {
                //cout<<"gone"<<endl;
                continue;
            }


            if(x_ab[1]<min_x)
            {
                //cout<<"gone"<<endl;
                continue;
            }
            else if(x_ab[1]>max_x)
            {
                x_ab[1]=max_x;
                clipped=true;
            }


            //compute za, zb at the clipped part if changed
            if(clipped)
            {
                z_ab[0]=z_ab[0]+(z_ab[0]-z_ab[1])*(x_ab[0]-prev_x[0])/(prev_x[0]-prev_x[1]);
                z_ab[1]=z_ab[1]+(z_ab[1]-z_ab[0])*(x_ab[1]-prev_x[1])/(prev_x[1]-prev_x[0]);
            }


            double xa,za,xb,zb;

            xa=x_ab[0];
            xb=x_ab[1];
            za=z_ab[0];
            zb=z_ab[1];

            int col_start = round((xa-leftX)/dx);////col starts from the left (xa)
            int col_end = round((xb-leftX)/dx);/////col ends at right side (xb)


            for(int col_no=col_start; col_no<=col_end; col_no++)//////start from the leftmost column containing the triangle
            {
                double xp=leftX+col_no*dx;/////x value of column

                double zp=za+(za-zb)*((xp-xa)/(xa-xb));/////find z value at xp

                if (zp<z_front_limit)//////ignore values in front of the limit
                    continue;

                if(zp<z_buffer[row_no][col_no])//////update with lower value
                {
                    z_buffer[row_no][col_no]=zp;
                    img->set_pixel(col_no, row_no, t.rgb[0], t.rgb[1], t.rgb[2]);
                }
            }
        }
    }
    for (int i=0; i<screenHeight; i++)
    {
        for (int j=0; j<screenWidth; j++)
        {
            if (z_buffer[i][j]<z_max)
            {
                fout<< setprecision(6)<<fixed<<z_buffer[i][j]<<"\t";////////print z_buffer values
            }
        }
        fout << endl;
    }

    fout.close();
    //cout<<"here"<<endl;

    img->save_image("out.bmp");

    z_buffer.clear();
    delete img;
    return 0;
}
