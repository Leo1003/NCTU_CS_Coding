#include"Quadratic.h"
#include<iostream>
using namespace std;

int main(){
	double a, b, c;
	cin >> a >> b >> c;
	Quadratic formula(a, b, c);
	formula.solver();
	return 0;
}
