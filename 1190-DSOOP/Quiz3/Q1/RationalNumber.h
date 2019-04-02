#ifndef RATIONAL_NUMBER_H
#define RATIONAL_NUMBER_H

#include <cstdlib>
#include <iostream>
using namespace std;
/*-------------------------*/
/*  write  your code here  */

int gcd(int a, int b)
{
    int t;
    while (b != 0) {
        t = b;
        b = a % b;
        a = t;
    }
    return a;
}

class RationalNumber {
public:
    RationalNumber();
    RationalNumber(int n, int d);
    friend RationalNumber operator+ (const RationalNumber &lhs, const RationalNumber &rhs);
    friend RationalNumber operator- (const RationalNumber &lhs, const RationalNumber &rhs);
    friend RationalNumber operator* (const RationalNumber &lhs, const RationalNumber &rhs);
    friend RationalNumber operator/ (const RationalNumber &lhs, const RationalNumber &rhs);
    bool operator< (const RationalNumber &rhs) const;
    bool operator> (const RationalNumber &rhs) const;
    bool operator>= (const RationalNumber &rhs) const;
    bool operator<= (const RationalNumber &rhs) const;
    bool operator== (const RationalNumber &rhs) const;
    bool operator!= (const RationalNumber &rhs) const;
    void printRational();
private:
    void reduce();
    int numerator, denominator;
};

RationalNumber::RationalNumber()
    :numerator(0), denominator(1) {}

RationalNumber::RationalNumber(int n, int d)
    :numerator(n), denominator(d) {}

RationalNumber operator+ (const RationalNumber &lhs, const RationalNumber &rhs)
{
    RationalNumber r;
    r.denominator = lhs.denominator * rhs.denominator;
    r.numerator = lhs.numerator * rhs.denominator + rhs.numerator * lhs.denominator;
    r.reduce();
    return r;
}

RationalNumber operator- (const RationalNumber &lhs, const RationalNumber &rhs)
{
    RationalNumber r;
    r.denominator = lhs.denominator * rhs.denominator;
    r.numerator = lhs.numerator * rhs.denominator - rhs.numerator * lhs.denominator;
    r.reduce();
    return r;
}

RationalNumber operator* (const RationalNumber &lhs, const RationalNumber &rhs)
{
    RationalNumber r;
    r.denominator = lhs.denominator * rhs.denominator;
    r.numerator = lhs.numerator * rhs.numerator;
    r.reduce();
    return r;
}

RationalNumber operator/ (const RationalNumber &lhs, const RationalNumber &rhs)
{
    RationalNumber r;
    r.denominator = lhs.denominator * rhs.numerator;
    r.numerator = lhs.numerator * rhs.denominator;
    r.reduce();
    return r;
}

bool RationalNumber::operator< (const RationalNumber &rhs) const
{
    RationalNumber l(*this), r(rhs);
    l.denominator *= rhs.denominator;
    l.numerator *= rhs.denominator;
    r.denominator *= this->denominator;
    r.numerator *= this->denominator;
    return l.numerator < r.numerator;
}

bool RationalNumber::operator> (const RationalNumber &rhs) const
{
    return !(*this < rhs) && !(*this == rhs);
}

bool RationalNumber::operator>= (const RationalNumber &rhs) const
{
    return !(*this < rhs);
}

bool RationalNumber::operator<= (const RationalNumber &rhs) const
{
    return !(*this > rhs);
}

bool RationalNumber::operator== (const RationalNumber &rhs) const
{
    RationalNumber l(*this), r(rhs);
    l.denominator *= rhs.denominator;
    l.numerator *= rhs.denominator;
    r.denominator *= this->denominator;
    r.numerator *= this->denominator;
    return l.numerator == r.numerator;
}

bool RationalNumber::operator!= (const RationalNumber &rhs) const
{
    return !(*this == rhs);
}

void RationalNumber::printRational()
{
    this->reduce();
    if (this->denominator == 1) {
        cout << this->numerator;
    } else {
        cout << this->numerator << "/" << this->denominator;
    }
}

void RationalNumber::reduce()
{
    int minus = 0;
    if (this->numerator < 0) {
        minus++;
    }
    if (this->denominator < 0) {
        minus++;
    }
    this->numerator = abs(this->numerator);
    this->denominator = abs(this->denominator);

    int n = gcd(this->numerator, this->denominator);
    this->numerator /= n;
    this->denominator /= n;

    if (minus % 2 == 1) {
        this->numerator = -this->numerator;
    }
}

/*--------------------------*/
#endif
