#include "eWallet.h"
#include<iomanip>
#include <iostream>
using namespace std;

int main(){
    User user("A123456789",'M');
	SavingsAccount saver(0, user);

    int input_code;
    float input_content;

    while(cin>>input_code>>input_content){
        if(input_code == 1){
            saver.deposit(input_content);
        }else if(input_code == 2){
            saver.withdraw(input_content);
        }else if(input_code == 3){
            saver.calculateMonthlyInterest();
        }else if(input_code == 4){
            SavingsAccount::modifyInterestRate(input_content);
        }else{
            break;
        }
        //cout << fixed << setprecision(5) << saver.GetBalance() << endl;
    }
	cout << fixed << setprecision(5) << saver.GetBalance();
	return 0;
}
