#include"Circle.h"
#include<iostream>
using namespace std;

int main(){
	int type = 0;
	float x = 0, y = 0, r = 0;
	//cout << "Enter type: ";
	cin >> type;
	if (type == 1){
		Circle c1;
		//cout << "Enter X, Y, Radius: ";
		cin >> x >> y >> r;
		c1.set_circle(x, y, r);
		c1.print();
	}
	if (type == 2){
		float x2 = 0, y2 = 0, r2 = 0;
		//cout << "Enter X, Y, Radius, X2, Y2, Radius2: ";
		cin >> x >> y >> r >> x2 >> y2 >> r2;
		Circle c1;
		Circle c2;
		c2.set_circle(x2, y2, r2);
		c1.set_circle(x, y, r, c2);
		c1.print();
		c2.print();
	}
	return 0;
}
