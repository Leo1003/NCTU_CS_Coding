#include<stdio.h>
#include<math.h>
#define PI 3.1415926
#include<iomanip>
#include<iostream>
using namespace std;

float distance(float x1, float y1, float x2, float y2)
{
    float x = fabs(x1 - x2);
    float y = fabs(y1 - y2);
    return sqrt(x*x + y*y);
}

class Circle{
    public:
        Circle();
        float cal_area();
        float cal_circumference();
        void set_circle(float , float , float );
        void set_circle(float , float , float , Circle);
        void print();
    private:
        float center_x;
        float center_y;
        float radius;
};

Circle::Circle()
{
    center_x = 0;
    center_y = 0;
    radius = 0;
}

float Circle::cal_area()
{
    return PI * radius * radius;
}

float Circle::cal_circumference()
{
    return 2 * PI * radius;
}

void Circle::set_circle(float x, float y, float r)
{
    center_x = x;
    center_y = y;
    radius = r;
}

void Circle::set_circle(float x, float y, float r, Circle c)
{
    float dis = distance(x, y, c.center_x, c.center_y);
    //Case3
    if (dis == c.radius) {
        *this = c;
    } else if (dis >= c.radius + r) {
        center_x = x;
        center_y = y;
        radius = r;
    } else {
        center_x = x;
        center_y = y;
        radius = fabs(dis - c.radius);
    }
}

void Circle::print()
{
    cout << setprecision(10) << center_x << "_" << center_y << "_" << radius << "_" << cal_area() << "_" << cal_circumference() << endl;
}
