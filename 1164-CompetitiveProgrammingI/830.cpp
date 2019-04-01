#include <algorithm>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <stdlib.h>
using namespace std;

inline double square(const double x)
{
    return x * x;
}

struct Vector {
    double x, y;

    Vector(): x(0), y(0) {}

    Vector(double x, double y): x(x), y(y) {}

    Vector operator- () const 
    {
        Vector v(-this->x, -this->y);
        return v;
    }

    Vector operator+ (const Vector &rhs) const 
    {
        Vector v(this->x + rhs.x, this->y + rhs.y);
        return v;
    }

    Vector operator- (const Vector &rhs) const 
    {
        Vector v(this->x - rhs.x, this->y - rhs.y);
        return v;
    }

    double operator* (const Vector &rhs) const 
    {
        return (this->x * rhs.x + this->y * rhs.y);
    }

    double operator() () const
    {
        return sqrt(x * x + y * y);
    }

    double square() const
    {
        return (x * x + y * y);
    }

    friend istream& operator>> (istream& stream, Vector& vector)
    {
        stream >> vector.x >> vector.y;
        return stream;
    }

    friend ostream& operator<< (ostream& stream, const Vector& vector)
    {
        stream << vector.x << " " << vector.y;
        return stream;
    }
};

double area(const Vector &v1, const Vector &v2)
{
    return sqrt(square(v1()) * square(v2()) - square(v1 * v2)) * 0.5;
}

Vector cal_circentre(const Vector &p1, const Vector &p2, const Vector &p3)
{
    Vector v1 = p2 - p1, v2 = p3 - p1;
    double m = (v1.x * v2.y - v1.y * v2.x) * 2.0;

    double Ox = (square(v1.x) * v2.y - square(v2.x) * v1.y + v1.y * v2.y * (v1.y - v2.y)) / m;
    double Oy = (v1.x * v2.x * (v2.x - v1.x) - square(v1.y) * v2.x + square(v2.y) * v1.x) / m;
    Vector center(Ox, Oy);
    return p1 + center;
}

Vector vector_mid(const Vector &p1, const Vector &p2)
{
    return Vector((p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0);
}

Vector cal_line_center(const Vector &p1, const Vector &p2, const Vector &p3)
{
    double xmx = max(max(p1.x, p2.x), p3.x);
    double xmn = min(min(p1.x, p2.x), p3.x);
    double ymx = max(max(p1.y, p2.y), p3.y);
    double ymn = min(min(p1.y, p2.y), p3.y);

    Vector vx(xmx, ymx), vn(xmn, ymn);
    return vector_mid(vx, vn);
}

Vector cal_obtuse(const Vector &p1, const Vector &p2, const Vector &p3)
{
    Vector v1 = p2 - p1, v2 = p3 - p1, v3 = p3 - p2;
    double a = v1(), b = v2(), c = v3();
    int index = 0;

    if (b > a && b > c) {
        index = 1;
    }
    if (c > a && c > b) {
        index = 2;
    }

    if (index == 0) {
        return vector_mid(p1, p2);
    } else if (index == 1) {
        return vector_mid(p1, p3);
    } else if (index == 2) {
        return vector_mid(p2, p3);
    }
    abort(); //unreachable
}

int trangle_type(const Vector &p1, const Vector &p2, const Vector &p3)
{
    Vector v1 = p2 - p1, v2 = p3 - p1, v3 = p3 - p2;
    double a = v1(), b = v2(), c = v3();
    double aq = v1.square(), bq = v2.square(), cq = v3.square();
    if (a >= b + c || b >= a + c || c >= a + b) {
        return -1;
    }

    if (aq == bq + cq || bq == aq + cq || cq == bq + aq) {
        return 1;
    } else if(aq > bq + cq || bq > aq + cq || cq > bq + aq) {
        return 2;
    } else {
        return 0;
    }
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    Vector p[3];
    while (cin >> p[0] >> p[1] >> p[2]) {
        Vector center;
        double radius = 0;
        switch (trangle_type(p[0], p[1], p[2])) {
            case 0:
            case 1:
                center = cal_circentre(p[0], p[1], p[2]);
                radius = (p[0] - center)();
                break;
            case 2:
                center = cal_obtuse(p[0], p[1], p[2]);
                radius = max(radius, (p[0] - center)());
                radius = max(radius, (p[1] - center)());
                radius = max(radius, (p[2] - center)());
                break;
            case -1:
                center = cal_line_center(p[0], p[1], p[2]);
                radius = max(radius, (p[0] - center)());
                radius = max(radius, (p[1] - center)());
                radius = max(radius, (p[2] - center)());
                break;
            default:
                abort();
        }
        cout << fixed << setprecision(6) << center << " " << radius << endl;
    }
    return 0;
}
