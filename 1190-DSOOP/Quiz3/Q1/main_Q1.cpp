#include <iostream>
#include <string> 
#include <vector>
#include <sstream>
#include "RationalNumber.h"
using namespace std;

//The function split the input fraction to numerator and denominator
// Eg: Input "5/3", Output arr[0]=5, arr[1]=3
vector<string> split(const string& str, const string& delim) {
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do{
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

int main() {
	char fraction1[100], fraction2[100];
	int denominator, numerator;
	
	cin >> fraction1;//Input the first fraction
	vector<string> fraction = split (fraction1, "/");
	numerator = atoi(fraction[0].c_str());
	denominator = atoi(fraction[1].c_str());
	RationalNumber f1(numerator, denominator);
	
	cin >> fraction2;//Input the second fraction
	fraction = split (fraction2, "/");
	numerator = atoi(fraction[0].c_str());
	denominator = atoi(fraction[1].c_str());
	RationalNumber f2(numerator, denominator);
	
    RationalNumber result;

	//Addition 
    result = f1 + f2;
    result.printRational();

	//Subtraction
    cout << '\n';
    result = f1 - f2;
    result.printRational();

	//Multiplication
    cout << '\n';
    result = f1 * f2;
    result.printRational();

	//Division
    cout << '\n';
    result = f1 / f2;
    result.printRational();

	//Rational(>)
    cout << '\n';
    f1.printRational();
    if(f1 > f2){
    	cout << ">"; 
	}
	else{
		cout << "<="; 
	}
    f2.printRational();

	//Rational(>=)
    cout << '\n';
    f1.printRational();
    if(f1 >= f2){
    	cout << ">="; 
	}
	else{
		cout << "<"; 
	}
    f2.printRational();

	//Equality(==)
    cout << '\n';
    f1.printRational();
    if(f1 == f2){
    	cout << "=="; 
	}
	else{
		cout << "!="; 
	}
    f2.printRational();
}
