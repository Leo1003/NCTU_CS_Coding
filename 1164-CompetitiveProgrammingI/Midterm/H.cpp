#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#define CARRY_THRE 1000000000

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
using namespace std;

class LargeNumber {
public: 
    LargeNumber()
        :data(15, 0)
    {
    }
    
    LargeNumber(string s)
        :data(15, 0)
    {
        for (size_t i = 0; i < s.size(); i += 9) {
            int t = 0, m = 1;
            for (size_t j = 0; j < 9 && i + j < s.size(); j++) {
                int d = s[s.size() - (i + j) - 1] - '0';
                t += m * d;
                m *= 10;
            }
            add_offset(t, i / 9);
        }
    }
    
    friend LargeNumber operator+ (const LargeNumber& a, const LargeNumber& b)
    {
        LargeNumber r;
        for (int i = 0; i < 12; i++) {
            r.add_offset(a.data[i] + b.data[i], i);
        }
        return r;
    }
    
    friend LargeNumber operator* (const LargeNumber& a, const LargeNumber& b)
    {
        LargeNumber r;
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 12; j++) {
                if (a.data[i] == 0 || b.data[j] == 0) {
                    continue;
                }
                r.add_offset(a.data[i] * b.data[j], i + j);
            }
        }
        return r;
    }
    
    void print()
    {
        bool flag = false;
        for (int i = data.size() - 1; i >= 0; i--) {
            //eprintf("data[%d] = %lld\n", i, data[i]);
            if (!flag) {
                if (data[i] > 0 || i == 0) {
                    printf("%lld", data[i]);
                    flag = true;
                }
            } else {
                printf("%09lld", data[i]);
            }
        }
    }
private:
    void add_offset(unsigned long long n, int offset)
    {
        data[offset] += n;
        for (int i = offset; i < 12; i++) {
            if (data[i] < CARRY_THRE) {
                break;
            }
            data[i + 1] += data[i] / CARRY_THRE;
            data[i] %= CARRY_THRE;
        }
    }
    vector<unsigned long long> data;
};

int read_digits(char *buf)
{
    int i = 0, c = 0;
    while ((c = getchar_unlocked()) != EOF) {
        if (c >= '0' && c <= '9') {
            buf[i++] = c;
            break;
        }
        if (c == '+' || c == '*') {
            buf[i++] = c;
            buf[i++] = '\0';
            return 0;
        }
        if (c == '\n') {
            buf[i++] = '\0';
            return -1;
        }
    }
    while ((c = getchar_unlocked()) != EOF) {
        if (c >= '0' && c <= '9') {
            buf[i++] = c;
        } else {
            ungetc(c, stdin);
            break;
        }
    }
    buf[i++] = '\0';
    return i;
}

int main()
{
    int T;
    char strbuf[1000];
    scanf("%d", &T);
    read_digits(strbuf);
    while (T--) {
        vector<LargeNumber> st;
        while (true) {
            int r = read_digits(strbuf);
            //eprintf("r = %d\n", r);
            if (r == -1) {
                break;
            } else if (r == 0) {
                LargeNumber a, b;
                switch (strbuf[0]) {
                    case '+':
                        b = st.back();
                        st.pop_back();
                        a = st.back();
                        st.pop_back();
                        st.push_back(a + b);
                        break;
                    case '*':
                        b = st.back();
                        st.pop_back();
                        a = st.back();
                        st.pop_back();
                        st.push_back(a * b);
                        break;
                }
            } else {
                string s(strbuf);
                LargeNumber num(s);
                st.push_back(num);
            }
        }
        st.front().print();
        printf("\n");
    }
    return 0;
}
