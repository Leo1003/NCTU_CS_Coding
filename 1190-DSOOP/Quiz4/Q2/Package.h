#ifndef PACKAGE_H
#define PACKAGE_H
using namespace std;
#include <iostream>

/* Write Your Code Here */
class Package {
protected:
    string name, address, city;
    int zipcode;
    double weight, cost, fee;
public:
    Package(string name, string address, string city, int zipcode, double weight, double cost)
        : name(name), address(address), city(city), zipcode(zipcode), weight(weight), cost(cost), fee(0)
    {}
    virtual double calculateCost();
    void setTransferFee(double fee);
};

double Package::calculateCost()
{
    return (weight * cost + fee);
}

void Package::setTransferFee(double fee)
{
    this->fee = fee;
}

class TwoDayPackage: public Package
{
private:
    double flat_fee;
public:
    TwoDayPackage(string name, string address, string city, int zipcode, double weight, double cost, double flat_fee)
        : Package(name, address, city, zipcode, weight, cost), flat_fee(flat_fee)
    {}
    double calculateCost();
};

double TwoDayPackage::calculateCost()
{
    return (weight * cost + fee + flat_fee);
}

class Human {
    string name, address, city;
    int zipcode;
public:
    Human(string name, string address, string city, int zipcode)
        : name(name), address(address), city(city), zipcode(zipcode)
    {}
    void print();   /*Don't modify this function*/
    string writeName();
    string writeAddress();
    string writeCity();
    int writeZipCode();
    int getZipCode();
};

void Human::print() { /*Don't modify this function*/
    cout << name <<endl;
    cout << address <<endl;
    cout << city <<endl;
    cout << zipcode <<endl;
}

string Human::writeName()
{
    return name;
}

string Human::writeAddress()
{
    return address;
}

string Human::writeCity()
{
    return city;
}

int Human::writeZipCode()
{
    return zipcode;
}

int Human::getZipCode()
{
    return zipcode;
}

/*----------------------*/
#endif




