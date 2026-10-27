#ifndef LAB1_BINARYTREE_H
#define LAB1_BINARYTREE_H

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

template <typename T, typename Compare = std::less<T>>
class BinarySearchTree {
private:
    struct Node {
        T value;

        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        Node* parent = nullptr;

        explicit Node(const T& item, Node* parentNode = nullptr)
            : value(item), parent(parentNode) {}
    };

    std::unique_ptr<Node> root;
    std::size_t size_ = 0;
    Compare compare_;

    // target должен быть непустым узлом этого дерева.
    std::unique_ptr<Node>& getOwner(Node* target) {
        if (target->parent == nullptr) {
            return root;
        }

        if (target->parent->left.get() == target) {
            return target->parent->left;
        }

        return target->parent->right;
    }

public:
    BinarySearchTree() = default;

    explicit BinarySearchTree(Compare compare)
        : compare_(std::move(compare)) {}

    ~BinarySearchTree() {
        clear();
    }

    BinarySearchTree(const BinarySearchTree&) = delete;
    BinarySearchTree& operator=(const BinarySearchTree&) = delete;

    bool empty() const noexcept {
        return size_ == 0;
    }

    std::size_t size() const noexcept {
        return size_;
    }

    void clear() noexcept {
        root.reset();
        size_ = 0;
    }

    bool contains(const T& target) const {
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

    bool append(const T& data) {
        if (!root) {
            root = std::make_unique<Node>(data, nullptr);
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
                    current->left =
                        std::make_unique<Node>(data, current);

                    ++size_;
                    return true;
                }
            }
            else if (compare_(current->value, data)) {
                if (current->right) {
                    current = current->right.get();
                }
                else {
                    current->right =
                        std::make_unique<Node>(data, current);

                    ++size_;
                    return true;
                }
            }
            else {
                return false;
            }
        }
    }

    bool erase(const T& target) {
        Node* current = root.get();

        while (current != nullptr) {
            if (compare_(target, current->value)) {
                current = current->left.get();
            }
            else if (compare_(current->value, target)) {
                current = current->right.get();
            }
            else {
                break;
            }
        }

        if (current == nullptr) {
            return false;
        }

        // Нет потомков или есть только один.
        if (!current->left || !current->right) {
            Node* parent = current->parent;
            std::unique_ptr<Node>& owner = getOwner(current);

            std::unique_ptr<Node> replacement;

            if (current->left) {
                replacement = std::move(current->left);
            }
            else {
                replacement = std::move(current->right);
            }

            if (replacement) {
                replacement->parent = parent;
            }

            owner = std::move(replacement);

            --size_;
            return true;
        }

        // Есть два потомка.
        Node* minimal = current->right.get();

        while (minimal->left) {
            minimal = minimal->left.get();
        }

        current->value = minimal->value;

        Node* parent_of_minimal = minimal->parent;

        std::unique_ptr<Node>& minimal_owner = getOwner(minimal);

        std::unique_ptr<Node> replacement =
            std::move(minimal->right);

        if (replacement) {
            replacement->parent = parent_of_minimal;
        }

        minimal_owner = std::move(replacement);

        --size_;
        return true;
    }
};

#endif // LAB1_BINARYTREE_H
