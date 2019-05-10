#include <iostream>
using namespace std;

/* Write Your Code Here */
class Account {
protected:
    double balance;
public:
    Account(double balance);
    virtual void credit(double amount);
    virtual void debit(double amount);
    double getBalance();
};

Account::Account(double balance)
{
    if (balance < 0) {
        this->balance = 0;
    } else {
        this->balance = balance;
    }
}

void Account::credit(double amount)
{
    double b = balance + amount;
    if (b >= 0) {
        balance = b;
    }
}

void Account::debit(double amount)
{
    double b = balance - amount;
    if (b >= 0) {
        balance = b;
    }
}

double Account::getBalance()
{
    return balance;
}

class SavingsAccount: public Account
{
private:
    double interest;
public:
    SavingsAccount(double balance, double interest);
    void calculateInterest();
};

SavingsAccount::SavingsAccount(double balance, double interest)
    : Account(balance)
{
    if (interest < 0) {
        this->interest = 0;
    } else {
        this->interest = interest;
    }
}

void SavingsAccount::calculateInterest()
{
    balance += interest * balance;
}

class CheckingAccount: public Account
{
private:
    double charge;
public:
    CheckingAccount(double balance, double charge);
    void credit(double amount);
    void debit(double amount);
};

CheckingAccount::CheckingAccount(double balance, double charge)
    : Account(balance)
{
    this->charge = charge;
}

void CheckingAccount::credit(double amount)
{
    double b = balance + amount - charge;
    if (b >= 0) {
        balance = b;
    }
}

void CheckingAccount::debit(double amount)
{
    double b = balance - amount - charge;
    if (b >= 0) {
        balance = b;
    }
}

/*----------------------*/
