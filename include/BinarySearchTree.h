//
// Created by bladerunner on 10/19/26.
//

#ifndef LAB1_BINARYTREE_H
#define LAB1_BINARYTREE_H




template <typename T, typename Compare = std::less<T>>

class BinarySearchTree {
private:
    struct Node {
        T value;
        Node* left = nullptr;
        Node* right = nullptr;

        explicit Node(const T& item): value(item){}
    };

    Node* root_ = nullptr;
    int size = 0;
    Compare compare_;

public:
    BinarySearchTree() = default;


};
#endif //LAB1_BINARYTREE_H
