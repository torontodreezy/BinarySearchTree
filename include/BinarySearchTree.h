//
// Created by bladerunner on 10/19/26.
//

#ifndef LAB1_BINARYTREE_H
#define LAB1_BINARYTREE_H
#include <cstddef>    // std::size_t
#include <functional> // std::less
#include <memory>
#include <utility>

using namespace std;

template <typename T, typename Compare = std::less<T>>

class BinarySearchTree {
private:
    struct Node {
        T value;
        unique_ptr<Node> left;
        unique_ptr<Node> right;

        explicit Node(const T& item) : value(item) {}
    };

    unique_ptr<Node> root;
    std::size_t size_ = 0;
    Compare compare_;

public:
    BinarySearchTree() = default;

    BinarySearchTree(Compare compare) : compare_(move(compare)) {}

    bool contains(const T& target) {
        const Node* current = root.get();
        while (current != nullptr) {
            if (compare_(target, current->value)) {
                current = current->left.get();
            }
            else if (compare_(current->value, target)) {
                current = current->right.get();
            }
            else {
                return true;
            }
        }
        return false;
    }


    bool empty() const noexcept {
        return size_ == 0;
    }

    bool append(const T& data) {
        if (root == nullptr) {
            root = std::make_unique<Node>(data);
            ++size_;
            return true;
        }

        Node* current = root.get();

        while (true) {
            if (compare_(data, current->value)) {
                if (current->left) {
                    current = current->left.get();
                }
                else {
                    current->left = std::make_unique<Node>(data);
                    ++size_;
                    return true;
                }
            }
            else if (compare_(current->value, data)) {
                if (current->right) {
                    current = current->right.get();
                }
                else {
                    current->right = std::make_unique<Node>(data);
                    ++size_;
                    return true;
                }
            }
            else {
                return false;
            }
        }
    }

    std::size_t size() const noexcept {
        return size_;
    }

    void clear() {
        root.reset();
        size_ = 0;
    }


    ~BinarySearchTree() {
        clear();
    }
};
#endif //LAB1_BINARYTREE_H
