#include <iostream>
#include "include/BinarySearchTree.h"

using namespace std;



int main() {
    BinarySearchTree<int> tree;

    for (int value : {50, 30, 70, 20, 40, 60, 80, 35, 45, 37}) {
        tree.append(value);
    }

   tree.erase(30);

    cout << tree.contains(70);
}