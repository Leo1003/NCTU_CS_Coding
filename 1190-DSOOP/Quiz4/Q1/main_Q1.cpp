#include "Account.h"
#include<iomanip>
#include <iostream>
#include <memory>
using namespace std;

int main(){
	    
    Account* account1 = NULL;
    SavingsAccount* account2 = NULL;
    CheckingAccount* account3 = NULL;
    
    double initial_amount = 0.0; //The balance of account initialization
    double interest_rate = 0.0; //The rate of interest
    double transaction_fee = 0.0; //The fee for each transaction
	double transaction_amount = 0.0; //The amount for debit and credit
	
	cin >> initial_amount;
    account1 = new Account( initial_amount );
        
    cin >> initial_amount >> interest_rate;
    account2 = new SavingsAccount( initial_amount, interest_rate );
    
    cin >> initial_amount >> transaction_fee;
    account3 = new CheckingAccount( initial_amount, transaction_fee );
 	
 	int input_account, input_instruction, input_content;
	while(cin >> input_account >> input_instruction >> input_content){
		
        if(input_account == 1){
        	
        	if(input_instruction ==1){
        		account1 -> debit( input_content );
			}else if(input_instruction == 2){
				account1 -> credit( input_content );
			}
            
        }else if(input_account == 2){
        	
        	if(input_instruction ==1){
        		account2 -> debit( input_content );
			}else if(input_instruction == 2){
				account2 -> credit( input_content );
			}else if(input_instruction == 3){
				account2 -> calculateInterest();
			}
			
        }else if(input_account == 3){
        	
        	if(input_instruction ==1){
        		account3 -> debit( input_content );
			}else if(input_instruction == 2){
				account3 -> credit( input_content );
			}
			
        }else{
        	break;
        }
        		
    }
    
    cout << "Account1 : $" << account1->getBalance() << endl;
  	cout << "Account2 : $" << account2->getBalance() << endl;
 	cout << "Account3 : $" << account3->getBalance();
    
    return 0;
}
