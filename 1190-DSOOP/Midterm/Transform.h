#define TRANSFORM_H
#ifdef TRANSFORM_H
#include <iostream>
#include <math.h>

using namespace std;
class Transform
{
public:
    Transform(double x, double y)
        : x(x), y(y)
    {}

    void set_polar(double *c, double *q)
    {
        *c = sqrt(x * x + y * y);
        *q = atan(y / x);
    }

    friend class Distance;
private:
    double x, y;
};

class Distance
{
public:
    Distance(double x, double y)
        : x(x), y(y)
    {}

    double get_distance(const Transform &t)
    {
        double dx, dy;
        dx = abs(t.x - x);
        dy = abs(t.y - y);
        return sqrt(dx * dx + dy * dy);
    }
private:
    double x, y;
};

#endif
