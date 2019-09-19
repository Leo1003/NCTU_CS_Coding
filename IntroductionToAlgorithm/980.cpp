#include <algorithm>
#include <stdio.h>
#include <vector>
using namespace std;

void recur(const vector<int> &arr, vector<int> &stack, size_t index)
{
    if (stack.size() == 6) {
        for (size_t i = 0; i < stack.size(); i++) {
            printf("%d ", stack[i]);
        }
        printf("\n");
        return;
    }
    
    for (size_t i = index + 1; i < arr.size(); i++) {
        stack.push_back(arr[i]);
        recur(arr, stack, i);
        stack.pop_back();
    }
}

int main()
{
    int k = -1;
    bool newline = false;
    while (true) {
        scanf("%d", &k);
        if (k <= 0) {
            break;
        }

        // Newline between each testdata
        if (newline) {
            printf("\n");
        }

        vector<int> S;
        S.reserve(k);
        for (int i = 0; i < k; i++) {
            int t;
            scanf("%d", &t);
            S.push_back(t);
        }
        
        vector<int> stack;
        stack.reserve(6);
        recur(S, stack, -1);
        newline = true;
    }
    return 0;
}

