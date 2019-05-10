#include <iostream>
#include <string>
#include "Package.h"
using namespace std;
int main()
{
	double Weight, Cost_per_Ounce, Cost, flatFee;
	int ZipCode, package_ZipCode;
	string Name, Address, City, package_Name, package_Address, package_City;
	
	//Enter info from Sender of class Human 
	cin >> Name >> Address >> City >> ZipCode >> Weight >> Cost_per_Ounce;
	Human Sender1(Name, Address, City, ZipCode);
	package_Name = Sender1.writeName();
	package_Address = Sender1.writeAddress();
	package_City = Sender1.writeCity();
	package_ZipCode = Sender1.writeZipCode();
	Package FromSender1(package_Name, package_Address, package_City, package_ZipCode, Weight, Cost_per_Ounce);
	
	//Enter info from Receiver of class Human
	cin >> Name >> Address >> City >> ZipCode;
	Human Receiver1(Name, Address, City, ZipCode);
	cout << endl;
	
	//Print out the cost and the Receiver info
	if (Sender1.getZipCode() != Receiver1.getZipCode()){
		cout << "******************" << endl;
		FromSender1.setTransferFee(100.0);
		cout << "Total cost: " << FromSender1.calculateCost() << endl;
		Receiver1.print();	
	}
	else{
		cout << "******************" << endl;
		cout << "Total cost: " << FromSender1.calculateCost() << endl;
		Receiver1.print();
	}
	cout << "******************" << endl;
	
	//Enter info from Sender of class Human
	cin >> Name >> Address >> City >> ZipCode >> Weight >> Cost_per_Ounce >> flatFee;
	Human Sender2(Name, Address, City, ZipCode);
	package_Name = Sender2.writeName();
	package_Address = Sender2.writeAddress();
	package_City = Sender2.writeCity();
	package_ZipCode = Sender2.writeZipCode();
	TwoDayPackage FromSender2(package_Name, package_Address, package_City, package_ZipCode, Weight, Cost_per_Ounce, flatFee);
	
	//Enter info from Receiver of class Human
	cin >> Name >> Address >> City >> ZipCode;
	Human Receiver2(Name, Address, City, ZipCode);
	cout << endl;
	
	//Print out the cost and the Receiver info
	if (Sender2.getZipCode() != Receiver2.getZipCode()){
		cout << "******************" << endl;
		FromSender2.setTransferFee(100.0);
		cout << "Total cost: " << FromSender2.calculateCost() << endl;
		Receiver2.print();
	}
	else{
		cout << "******************" << endl;
		cout << "Total cost: " << FromSender2.calculateCost() << endl;
		Receiver2.print();
	}
	cout << "******************" << endl;
}
