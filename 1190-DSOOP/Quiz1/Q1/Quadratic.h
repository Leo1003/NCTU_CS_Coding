#include<math.h>
#include<iostream>
using namespace std;

class Quadratic{
    public:
        Quadratic(double, double, double);
        void solver();
        void print();
    private:
        double a;
        double b;
        double c;
        double real, imag;
};

Quadratic::Quadratic(double a, double b, double c)
{
    this->a = a;
    this->b = b;
    this->c = c;
    this->real = 0;
    this->imag = 0;
}

void Quadratic::solver()
{
    double x = (-b);
    double y = (b * b) - (4 * a * c);
    if (y < 0) {
        this->imag = sqrt(-y) / (2*a);
        this->real = x / (2*a);
    } else {
        this->imag = 0;
        this->real = (x + sqrt(y)) / (2*a);
    }
    print();
    if (y < 0) {
        this->imag = (-sqrt(-y)) / (2*a);
        this->real = x/(2*a);
    } else {
        this->imag = 0;
        this->real = (x - sqrt(y)) / (2*a);
    }
    print();
}

void Quadratic::print()
{
    cout << real << "_" << imag << endl;
}
