#include <iostream>
#include <vector>
#include <memory>
using namespace std;


int BinarySearch(const vector<int>& v, int x) {
    const int size = v.size();
    int left = 0;
    int right = size-1;
    while (left <= right) {
        int mid = left + (right-left)/2;
        if (v[mid] == x) {
            return mid;
        }
        if (x < v[mid]) {
            right = mid-1;
        }
        if (x > v[mid]){
            left = mid + 1;
        }
    }
    return -1;
}

int search(const vector<int>& v, int x) {
    const int size = v.size();
    for (int i = 0; i < size; i++) {
        if (v[i] == x) {
            return i;
        }
    }
    return -1;
}

int main() {




}


