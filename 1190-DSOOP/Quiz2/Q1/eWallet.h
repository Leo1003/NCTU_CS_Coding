#include<iostream>
using namespace std;

class User{
    public:
        User(){};
        User(string _id, char _gender);
        ~User(){};
    private:
        string id;
        char gender;
};

class SavingsAccount{
    public:
        SavingsAccount(){};
        SavingsAccount(float AccountValue, const User&);
        ~SavingsAccount(){};
        static float annualInterestRate;
        void calculateMonthlyInterest();
        static void modifyInterestRate(float InterestRate);
        void deposit(float cash);
        void withdraw(float cash);
        float GetBalance() const { return savingsBalance; }
    private:
        float savingsBalance;
        User user;
};

User::User(string _id, char _gender)
    :id(_id), gender(_gender) {}

float SavingsAccount::annualInterestRate = 0.12;

SavingsAccount::SavingsAccount(float AccountValue, const User &u)
{
    savingsBalance = AccountValue;
    user = u;
}

void SavingsAccount::modifyInterestRate(float InterestRate)
{
    annualInterestRate = InterestRate;
}

void SavingsAccount::calculateMonthlyInterest()
{
    savingsBalance *= (1.0 + (annualInterestRate / 12.0));
}

void SavingsAccount::deposit(float cash)
{
    savingsBalance += cash;
}

void SavingsAccount::withdraw(float cash)
{
    savingsBalance -= cash;
}
