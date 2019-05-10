#include "Person.h"
#include <iostream>
using namespace std;

int main()
{
    string s_name, t_name, g_name, t_title, g_title;
    char s_sex, t_sex, g_sex;
    int s_age, t_age, g_age;
    float s_score, g_score, g_wage;
    cin >> s_name >> s_sex >> s_age >> s_score;
    cin >> t_name >> t_sex >> t_age >> t_title;
    cin >> g_name >> g_sex >> g_age >> g_title >> g_score >> g_wage;

    Student std(s_name, s_sex, s_age, s_score);
    std.display();
    Teacher tcr(t_name, t_sex, t_age, t_title);
    tcr.display();
    Graduate grad1(g_name, g_sex, g_age, g_title, g_score, g_wage);
    cout << grad1 << endl;
    cout << --grad1 << endl;
    cout << ++grad1 << endl;
    cout << grad1-- << endl;
    cout << grad1++ << endl;
    return 0;
}