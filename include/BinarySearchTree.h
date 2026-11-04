#ifndef LAB1_BINARYTREE_H
#define LAB1_BINARYTREE_H

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <stack>

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

    std::unique_ptr<Node> clone(const Node* source, Node* parent) {
        if (source == nullptr) {
            return nullptr;
        }
        auto copy = std::make_unique<Node>(source->value, parent);
        copy->left = clone(source->left.get(), copy.get());
        copy->right = clone(source->right.get(), copy.get());

        return copy;
    }

public:
    BinarySearchTree() = default;

    explicit BinarySearchTree(Compare compare)
        : compare_(std::move(compare)) {}

    ~BinarySearchTree() {
        clear();
    }

    //move конструктор
    BinarySearchTree(BinarySearchTree&& other) noexcept
        : root(std::move(other.root)),
          size_(std::exchange(other.size_, 0)),
          compare_(std::move(other.compare_)) {}

    BinarySearchTree& operator=(BinarySearchTree&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        clear();
        compare_ = std::move(other.compare_);
        root = std::move(other.root);
        size_ = std::exchange(other.size_, 0);

        return *this;
    }

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

    bool insert(const T& data) {
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

    // объявление метода map
    template <typename Func>
    BinarySearchTree map(Func func) const;

    // объявление метода where
    template <typename Predicate>
    BinarySearchTree where(Predicate lambda) const;

    // объявление метода reduce
    template <typename Accumulator, typename func>
    Accumulator reduce(Accumulator start, func lambda) const;

};

//реализация map
template <typename T, typename Compare>
template <typename Func>
BinarySearchTree<T, Compare>
BinarySearchTree<T, Compare>::map(Func func) const {
    BinarySearchTree<T, Compare> result(compare_);
    std::stack<const Node*> nodes;
    if (root) {
        nodes.push(root.get());
    }

    while (!nodes.empty()) {
        const Node* current = nodes.top();
        nodes.pop();
        result.insert(func (current->value));
        if (current->right.get() != nullptr) {
            nodes.push(current->right.get());
        }
        if (current->left.get() != nullptr) {
            nodes.push(current->left.get());
        }
    }
    return result;
}

//реализация where

template <typename T, typename Compare>
template <typename Predicate>
BinarySearchTree<T, Compare>
BinarySearchTree<T, Compare>::where(Predicate lambda) const {
    BinarySearchTree<T, Compare> result(compare_);
    std::stack<const Node*> nodes;
    // обход
    if (root) {
        nodes.push(root.get());
    }
    while (!nodes.empty()) {
        const Node* current = nodes.top();
        nodes.pop();
        if (lambda(current->value)) {
            result.insert(current->value);
        }
        if (current->right.get() != nullptr) {
            nodes.push(current->right.get());
        }
        if (current->left.get() != nullptr) {
            nodes.push(current->left.get());
        }
    }
    return result;
}

template <typename T, typename Compare>
template <typename Accumulator, typename func>
Accumulator
BinarySearchTree<T, Compare>::reduce(Accumulator start, func lambda) const {
    Accumulator result = start;
    std::stack<const Node*> nodes;
    if (root.get()) {
        nodes.push(root.get());
    }
    while (!nodes.empty()) {
        const Node* current = nodes.top();
        nodes.pop();
        result = lambda(result, current->value);
        if (current->right.get() != nullptr) {
            nodes.push(current->right.get());
        }
        if (current->left.get() != nullptr) {
            nodes.push(current->left.get());
        }

    }
    return result;
}


#endif // LAB1_BINARYTREE_H
