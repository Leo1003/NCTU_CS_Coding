#include "IntegerSet.h"
#include <iostream>
using namespace std;

int main()
{
	const int count=10;
	int size,element,h_element,l_element;
	cin >> size;
	IntegerSet i_set(size);
	EvenIntegerSet e_set(size);
	for (int i=0; i<count; i++){
		cin >> element;
		i_set.InsertElement(element);
		e_set.InsertElement(element);
	}
    i_set.printSet();
    cout<< endl;
    e_set.filterSet();
    cout<< endl;
    h_element = highest(&i_set,size);
    cout << h_element << endl;
    l_element = lowest(&i_set,size);
    cout << l_element;
    return 0;
}


