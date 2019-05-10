#include<iostream>
#include<iomanip>
#include"function.h"
using namespace std;

int main()
{
	int i_list[15],check,i_sum;
	double d_list[15],d_sum=0;
	cin >> check;
	if (check==0){
		for (int i=0;i<15;i++)
			cin >> i_list[i];
		cin >> i_sum;
		cout<<findsum(i_list,i_sum)<<endl;
	}
	else{
		for (int i=0;i<15;i++)
			cin >> d_list[i];
		cin >> d_sum;
		cout<<findsum(d_list,d_sum)<<endl;
	}
	return 0;
}









