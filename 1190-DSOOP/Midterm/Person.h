#define PERSON_H
#ifdef PERSON_H
#include <iostream>
#include <string>
#include <cstring>

using namespace std;

class Person {
public:
    Person(const string &name, char sex, int age)
        : name(name), sex(sex), age(age)
    {}
protected:
    string name;
    char sex;
    int age;
};

class Student: public virtual Person {
public:
    Student(const string &name, char sex, int age, float score)
        : Person(name, sex, age), score(score)
    {}

    void display()
    {
        cout << name << "_" << sex << "_" << age << "_" << score << endl;
    }
protected:
    float score;
};

class Teacher: public virtual Person {
public:
    Teacher(const string &name, char sex, int age, const string &title)
        : Person(name, sex, age), title(title)
    {}

    void display()
    {
        cout << name << "_" << sex << "_" << age << "_" << title << endl;
    }
protected:
    string title;
};

class Graduate: public Teacher, public Student {
public:
    Graduate(const string &name, char sex, int age, const string &title, float score, float wage)
        : Person(name, sex, age), Teacher(name, sex, age, title), Student(name, sex, age, score), wage(wage)
    {}

    Graduate operator++()
    {
        this->wage += 1;
        return *this;
    }

    Graduate operator++(int)
    {
        Graduate tmp(*this);
        operator ++();
        return tmp;
    }

    Graduate operator--()
    {
        this->wage -= 1;
        return *this;
    }

    Graduate operator--(int)
    {
        Graduate tmp(*this);
        operator --();
        return tmp;
    }

    friend ostream& operator<<(ostream& stream, const Graduate& grad)
    {
        stream << grad.name << "_" << grad.sex << "_" << grad.age << "_" << grad.score << "_" << grad.title << "_" << grad.wage;
        return stream;
    }
private:
    float wage;
};

#endif
