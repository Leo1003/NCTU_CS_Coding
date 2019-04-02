#ifndef MYTIME_H
#define MYTIME_H
#include <iostream>

class Mytime
{
public:
    Mytime() :hour(0), minute(0) {}
    Mytime(int h, int m) :hour(h), minute(m)
    {
        normalize();
    }
    friend Mytime operator+ (const Mytime &lhs, const Mytime &rhs);
    friend Mytime operator- (const Mytime &lhs, const Mytime &rhs);
    friend Mytime operator* (const Mytime &lhs, int rhs);
    friend Mytime operator* (int lhs, const Mytime &rhs);

    Mytime& operator++ ();
    Mytime operator++ (int);

    friend std::ostream& operator<< (std::ostream& stream, const Mytime &rhs);
private:
    void normalize();
    int hour, minute;
};

Mytime operator+ (const Mytime &lhs, const Mytime &rhs)
{
    Mytime t(lhs.hour + rhs.hour, lhs.minute + rhs.minute);
    return t;
}

Mytime operator- (const Mytime &lhs, const Mytime &rhs)
{
    Mytime t(lhs.hour - rhs.hour, lhs.minute - rhs.minute);
    return t;
}

Mytime operator* (const Mytime &lhs, int rhs)
{
    Mytime t(lhs.hour * rhs, lhs.minute * rhs);
    return t;
}

Mytime operator* (int lhs, const Mytime &rhs)
{
    Mytime t(lhs * rhs.hour, lhs * rhs.minute);
    return t;
}

Mytime& Mytime::operator++ ()
{
    this->minute++;
    normalize();
    return *this;
}

Mytime Mytime::operator++ (int)
{
    Mytime ret(*this);
    ++(*this);
    return ret;
}

std::ostream& operator<< (std::ostream& stream, const Mytime &rhs)
{
    stream << rhs.hour << "_";
    if (rhs.hour >= 2) {
        stream << "hours";
    } else {
        stream << "hour";
    }
    stream << "_" << rhs.minute << "_";
    if (rhs.minute >= 2) {
        stream << "minutes";
    } else {
        stream << "minute";
    }
    return stream;
}

void Mytime::normalize()
{
    while (this->minute < 0 && this->hour > 0) {
        this->hour--;
        this->minute += 60;
    }
    this->hour += this->minute / 60;
    this->minute %= 60;
}

#endif