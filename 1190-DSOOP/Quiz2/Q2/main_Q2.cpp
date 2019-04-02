#include "IntegerSet.h"
#include <iostream>
using namespace std;


int main()
{ 
	int insert_1,insert_2,insert_3,insert_4,insert_5,insert_6;
	IntegerSet testset1(100),testset2(100),testset3(100),testset4(100);
	cout << "Please choose six numbers to insert for testset1" <<endl;
	cin >> insert_1 >> insert_2 >>insert_3 >>insert_4 >> insert_5 >>insert_6;
    testset1.InsertElement(insert_1);
    testset1.InsertElement(insert_2);
    testset1.InsertElement(insert_3);
    testset1.InsertElement(insert_4);
    testset1.InsertElement(insert_5);
    testset1.InsertElement(insert_6);
    cout<< "--------------------------------" << endl;
    cout << "Set result for testset1" <<endl;
    testset1.printSet();
    cout<<endl;
    cout<< "--------------------------------" << endl;
    testset2.InsertElement(3);
    testset2.InsertElement(5);
    testset2.InsertElement(6);
    testset2.InsertElement(8);
    testset2.InsertElement(9);
    testset2.InsertElement(44);
    cout << "Set result for testset2" <<endl;
    testset2.printSet();
    cout<<endl;
    cout<< "--------------------------------" << endl;
    cout << "Set result for unionset" <<endl;
    testset3 = testset1.Union_of_Set(testset2);
    testset3.printSet();
    cout <<endl;
    cout<< "--------------------------------" << endl;
    cout << "Set result for intersectionset" <<endl;
    testset4 = testset1.Intersection_of_Set(testset2);
    testset4.printSet();
    cout <<endl;
    cout<< "--------------------------------" << endl;
    if (testset1.isEqualTo(testset2)){
    	cout << 1 << endl;
	}
	else{
		cout << 0 << endl;
	}
    return 0;
}
