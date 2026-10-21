#include <iostream>
#include <vector>
#include "include/BinarySearchTree.h"
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

    BinarySearchTree<int> tree;
    tree.append(1);
    tree.append(4);
    tree.append(1);
    tree.append(7);
    int x = tree.size();
    bool result = tree.contains(1);

    cout << x;
}


