#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

void print_vector(vector<int> &v) {
    for (int i: v) {
        cout << i << " ";
    }
    cout << endl;
}

int main() 
{
    int n, k;
    vector<int> arr;
    cin >> n >> k;
    for (int i = 0; i < n; i++) {
        int t;
        cin >> t;
        arr.push_back(t);
    }

    
    if (!next_permutation(arr.begin(), arr.end())) {
        cout << "hello world!" << endl;
    } else {
        print_vector(arr);
        for (int i = 1; i < k; i++) {
            if (next_permutation(arr.begin(), arr.end())) {
                print_vector(arr);
            } else {
                break;
            }
        }
    }
    return 0;
}
