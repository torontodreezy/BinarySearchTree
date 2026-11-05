#ifndef LAB1_BINARYTREE_H
#define LAB1_BINARYTREE_H

#include <type_traits>
#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <stack>
#include <stdexcept>

template <typename T, typename Compare = std::less<T>>
class BinarySearchTree {
private:
    struct Node {
        T value;

        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;

        std::weak_ptr<Node> parent;

        explicit Node(const T& item, const std::shared_ptr<Node>& parentNode = nullptr)
            : value(item), parent(parentNode) {}
    };

    std::shared_ptr<Node> root;
    std::size_t size_ = 0;
    Compare compare_;

    // target должен быть непустым узлом этого дерева.
    std::shared_ptr<Node>& getOwner(const std::shared_ptr<Node>& target) {
        auto parent = target->parent.lock();
        if (!parent) {
            return root;
        }

        if (parent->left == target) {
            return parent->left;
        }

        return parent->right;
    }

    std::shared_ptr<Node> clone(const std::shared_ptr<const Node>& source, const std::shared_ptr<Node>& parent,
                                std::size_t& count) const {
        if (source == nullptr) {
            return nullptr;
        }

        auto copy = std::make_shared<Node>(source->value, parent);
        count++;
        copy->left = clone(source->left, copy, count);

        copy->right = clone(source->right, copy, count);

        return copy;
    }

    //private метод обход + лямбда
    void traverse(const std::function<void(const T&)>& action) const {
        std::stack<const Node*> nodes;
        if (!root) {
            return;
        }
        nodes.push(root.get());

        while (!nodes.empty()) {
            const Node* current = nodes.top();
            nodes.pop();
            action(current->value);
            if (current->right.get() != nullptr) {
                nodes.push(current->right.get());
            }
            if (current->left.get() != nullptr) {
                nodes.push(current->left.get());
            }
        }
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
        std::shared_ptr<const Node> current = root;

        while (current != nullptr) {
            if (compare_(target, current->value)) {
                current = current->left;
            }
            else if (compare_(current->value, target)) {
                current = current->right;
            }
            else {
                return true;
            }
        }

        return false;
    }

    bool insert(const T& data) {
        if (!root) {
            root = std::make_shared<Node>(data, nullptr);
            ++size_;
            return true;
        }

        std::shared_ptr<Node> current = root;

        while (true) {
            if (compare_(data, current->value)) {
                if (current->left) {
                    current = current->left;
                }
                else {
                    current->left =
                        std::make_shared<Node>(data, current);

                    ++size_;
                    return true;
                }
            }
            else if (compare_(current->value, data)) {
                if (current->right) {
                    current = current->right;
                }
                else {
                    current->right =
                        std::make_shared<Node>(data, current);

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
        std::shared_ptr<Node> current = root;

        while (current != nullptr) {
            if (compare_(target, current->value)) {
                current = current->left;
            }
            else if (compare_(current->value, target)) {
                current = current->right;
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
            auto parent = current->parent.lock();
            std::shared_ptr<Node>& owner = getOwner(current);

            std::shared_ptr<Node> replacement;

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
        std::shared_ptr<Node> minimal = current->right;

        while (minimal->left) {
            minimal = minimal->left;
        }

        current->value = minimal->value;

        auto parent_of_minimal = minimal->parent.lock();

        std::shared_ptr<Node>& minimal_owner = getOwner(minimal);

        std::shared_ptr<Node> replacement =
            std::move(minimal->right);

        if (replacement) {
            replacement->parent = parent_of_minimal;
        }

        minimal_owner = std::move(replacement);

        --size_;
        return true;
    }

    // ограничения на func
    // тз описание системы, требования
    // лр 2 выбрать структуру с пояснением


    //map
    template <typename Func>
    BinarySearchTree map(Func func) const {
        static_assert(
            std::is_invocable_r_v<T, Func&, const T&>,
            "map: incompatible function signature"
        );
        BinarySearchTree result(compare_);
        traverse([&](const T& value) {
            result.insert(func(value));
        });
        return result;
    }
    // объявление метода where
    template <typename Func>
    BinarySearchTree where(Func predicate) const {
        static_assert(
            std::is_invocable_r_v<bool, Func&, const T&>,
            "where: incompatible function signature"
        );
        BinarySearchTree result(compare_);

        traverse([&](const T& value) {
            if (predicate(value)) {
                result.insert(value);
            }
        });
        return result;
    }

    //reduce
    template <typename Accumulator, typename Func>
Accumulator reduce(Accumulator start, Func func) const {
        static_assert(
            std::is_invocable_r_v<
                Accumulator,
                Func&,
                Accumulator,
                const T&
            >,
            "reduce: incompatible function signature"
        );

        Accumulator result = start;

        traverse([&](const T& value) {
            result = func(result, value);
        });

        return result;
    }

    // объявление метода получения поддерева
    BinarySearchTree extractSubtree(const T& target) const;
};


template <typename T, typename Compare>
BinarySearchTree<T, Compare>
BinarySearchTree<T, Compare>::extractSubtree(const T& target) const {
    BinarySearchTree<T, Compare> result(compare_);
    std::size_t count = 0;
    std::shared_ptr<const Node> current = nullptr;
    if (root) {
        current = root;
    }
    //поиск нужной ноды
    while (current != nullptr) {
        if (compare_(current->value, target)) {
            current = current->right;
        }
        else if (compare_(target, current->value)) {
            current = current->left;
        }
        else {
            break;
        }
    }
    result.root = clone(current, nullptr, count);
    result.size_ = count;
    return result;
}
#endif // LAB1_BINARYTREE_H
