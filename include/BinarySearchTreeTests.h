#ifndef LAB1_BINARY_SEARCH_TREE_TESTS_H
#define LAB1_BINARY_SEARCH_TREE_TESTS_H

#include "BinarySearchTree.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace bst_tests {

// Параметры и ожидаемые результаты задаются вручную ниже, в runTests().
// Например, замените ожидаемый размер 3 на 4 в тесте size:
// этот тест выдаст FAIL, остальные продолжат выполняться.

template <typename T>
inline std::string show(const T& value) {
    std::ostringstream out;
    out << std::boolalpha << value;
    return out.str();
}

template <typename T, typename U>
inline void expect(const T& actual, const U& expected, const char* description) {
    if (actual != expected) {
        throw std::runtime_error(std::string(description) + ": ожидалось " +
                                 show(expected) + ", получено " + show(actual));
    }
}

template <typename Compare>
inline void expectValues(const BinarySearchTree<int, Compare>& tree,
                         const std::vector<int>& expected) {
    expect(tree.size(), expected.size(), "Размер дерева");
    for (int value : expected) {
        if (!tree.contains(value)) {
            throw std::runtime_error("Отсутствует ожидаемый элемент " + show(value));
        }
    }
}

inline BinarySearchTree<int> makeTree(const std::vector<int>& input) {
    BinarySearchTree<int> tree;
    for (int value : input) tree.insert(value);
    return tree;
}

struct Order {
    bool descending;
    bool operator()(int a, int b) const { return descending ? a > b : a < b; }
};

struct Tracked {
    inline static int alive = 0;
    int value;
    explicit Tracked(int value) : value(value) { ++alive; }
    Tracked(const Tracked& other) : value(other.value) { ++alive; }
    Tracked& operator=(const Tracked&) = default;
    ~Tracked() { --alive; }
    bool operator<(const Tracked& other) const { return value < other.value; }
};

struct Result {
    std::string name;
    std::string method;
    std::string error;
    bool passed;
};

class Runner {
    std::vector<Result> results;
public:
    template <typename Test>
    void run(const std::string& name, const char* method, Test test) {
        Result result{name, method, {}, false};
        try {
            test();
            result.passed = true;
        } catch (const std::exception& error) {
            result.error = error.what();
        } catch (...) {
            result.error = "Неизвестное исключение";
        }
        results.push_back(result);
    }

    void print() const {
        std::size_t passed = 0;
        for (const auto& result : results) passed += result.passed;
        std::cout << "Пройдено: " << passed << '/' << results.size() << '\n';
        for (const auto& result : results) {
            std::cout << (result.passed ? "OK: " : "FAIL: ") << result.name;
            if (!result.passed) {
                std::cout << " — " << result.error << " — \"" << result.method << '"';
            }
            std::cout << '\n';
        }
    }
};

} // namespace bst_tests


inline void runTests() {
    using namespace bst_tests;
    Runner tests;

    tests.run("Конструктор пустого дерева", "BinarySearchTree()", [] {
        const std::size_t expectedSize = 0;
        const bool expectedEmpty = true;
        BinarySearchTree<int> tree;
        expect(tree.size(), expectedSize, "Размер");
        expect(tree.empty(), expectedEmpty, "Пустота");
    });

    struct EmptyCase { std::vector<int> input; bool expected; };
    const std::vector<EmptyCase> emptyCases = {
        {{}, true}, {{8}, false}
    };
    for (std::size_t i = 0; i < emptyCases.size(); ++i) {
        const auto& p = emptyCases[i];
        tests.run("empty #" + show(i + 1), "bool empty() const noexcept", [&] {
            auto tree = makeTree(p.input);
            expect(tree.empty(), p.expected, "Пустота");
        });
    }

    tests.run("size", "std::size_t size() const noexcept", [] {
        const std::vector<int> input = {8, 3, 10};
        const std::size_t expectedSize = 3; // Поставьте 4, чтобы получить FAIL.
        auto tree = makeTree(input);
        expect(tree.size(), expectedSize, "Размер");
    });

    struct InsertCase {
        std::vector<int> input;
        int value;
        bool expectedReturn;
        std::vector<int> expectedValues;
    };
    const std::vector<InsertCase> insertCases = {
        {{}, 8, true, {8}},
        {{8}, 3, true, {8, 3}},
        {{8}, 10, true, {8, 10}},
        {{8, 3, 10}, 3, false, {8, 3, 10}}
    };
    for (std::size_t i = 0; i < insertCases.size(); ++i) {
        const auto& p = insertCases[i];
        tests.run("insert #" + show(i + 1), "bool insert(const T& data)", [&] {
            auto tree = makeTree(p.input);
            expect(tree.insert(p.value), p.expectedReturn, "Результат вставки");
            expectValues(tree, p.expectedValues);
        });
    }

    struct ContainsCase { std::vector<int> input; int target; bool expected; };
    const std::vector<ContainsCase> containsCases = {
        {{}, 8, false}, {{8, 3, 10, 1, 6}, 8, true},
        {{8, 3, 10, 1, 6}, 1, true}, {{8, 3, 10, 1, 6}, 10, true},
        {{8, 3, 10, 1, 6}, 5, false}
    };
    for (std::size_t i = 0; i < containsCases.size(); ++i) {
        const auto& p = containsCases[i];
        tests.run("contains #" + show(i + 1), "bool contains(const T& target) const", [&] {
            auto tree = makeTree(p.input);
            expect(tree.contains(p.target), p.expected, "Результат поиска");
        });
    }

    struct EraseCase {
        std::vector<int> input;
        int target;
        bool expectedReturn;
        std::vector<int> expectedValues;
    };
    const std::vector<EraseCase> eraseCases = {
        {{}, 8, false, {}},
        {{8, 3, 10}, 42, false, {8, 3, 10}},
        {{8}, 8, true, {}},
        {{8, 3, 10}, 3, true, {8, 10}},
        {{8, 3, 10}, 10, true, {8, 3}},
        {{8, 3}, 8, true, {3}},
        {{8, 10}, 8, true, {10}},
        {{8, 3, 1}, 3, true, {8, 1}},
        {{8, 3, 6}, 3, true, {8, 6}},
        {{8, 10, 9}, 10, true, {8, 9}},
        {{8, 10, 14}, 10, true, {8, 14}},
        {{8, 3, 10, 14}, 8, true, {3, 10, 14}},
        {{8, 3, 14, 10, 12}, 8, true, {3, 14, 10, 12}},
        {{20, 8, 3, 14, 10, 12, 30}, 8, true, {20, 3, 14, 10, 12, 30}}
    };
    for (std::size_t i = 0; i < eraseCases.size(); ++i) {
        const auto& p = eraseCases[i];
        tests.run("erase #" + show(i + 1), "bool erase(const T& target)", [&] {
            auto tree = makeTree(p.input);
            expect(tree.erase(p.target), p.expectedReturn, "Результат удаления");
            expectValues(tree, p.expectedValues);
            // Удаляем оставшиеся элементы: проверяем обновление parent.
            for (int value : p.expectedValues) {
                expect(tree.erase(value), true, "Повторное удаление");
                expect(tree.contains(value), false, "Наличие удалённого элемента");
            }
            expect(tree.size(), std::size_t{0}, "Размер после всех удалений");
        });
    }

    tests.run("clear и повторное использование", "void clear() noexcept", [] {
        const std::vector<int> input = {8, 3, 10};
        const std::vector<int> expectedAfterClear = {};
        const int insertedAfterClear = 5;
        const std::vector<int> expectedAfterInsert = {5};
        auto tree = makeTree(input);
        tree.clear();
        expectValues(tree, expectedAfterClear);
        tree.clear();
        expectValues(tree, expectedAfterClear);
        tree.insert(insertedAfterClear);
        expectValues(tree, expectedAfterInsert);
    });

    tests.run("Перемещающий конструктор", "BinarySearchTree(BinarySearchTree&& other) noexcept", [] {
        const std::vector<int> input = {8, 3, 10};
        const std::vector<int> expectedDestination = {8, 3, 10};
        const std::vector<int> expectedSource = {};
        auto source = makeTree(input);
        BinarySearchTree<int> destination(std::move(source));
        expectValues(destination, expectedDestination);
        expectValues(source, expectedSource);
        source.insert(42);
        expectValues(source, {42});
        expectValues(destination, expectedDestination);
    });

    tests.run("Перемещающее присваивание", "BinarySearchTree& operator=(BinarySearchTree&& other) noexcept", [] {
        const std::vector<int> input = {8, 3, 10};
        const std::vector<int> oldDestination = {100, 200};
        const std::vector<int> expectedDestination = {8, 3, 10};
        const std::vector<int> expectedSource = {};
        auto source = makeTree(input);
        auto destination = makeTree(oldDestination);
        expect(&(destination = std::move(source)), &destination, "Возвращённый адрес");
        expectValues(destination, expectedDestination);
        expectValues(source, expectedSource);
        source.insert(42);
        expectValues(source, {42});
        auto* same = &destination;
        destination = std::move(*same);
        expectValues(destination, expectedDestination);
        destination = makeTree({});
        expectValues(destination, {});
    });

    struct MapCase {
        std::vector<int> input;
        int multiplier;
        int offset;
        std::vector<int> expected;
        int expectedCalls;
    };
    const std::vector<MapCase> mapCases = {
        {{8, 3, 10, 1, 6}, -1, 0, {-8, -3, -10, -1, -6}, 5},
        {{8, 3, 10}, 0, 7, {7}, 3},
        {{8, 3, 10}, 1, 0, {8, 3, 10}, 3},
        {{}, 2, 1, {}, 0}
    };
    for (std::size_t i = 0; i < mapCases.size(); ++i) {
        const auto& p = mapCases[i];
        tests.run("map #" + show(i + 1), "BinarySearchTree map(Func func) const", [&] {
            const auto tree = makeTree(p.input);
            int calls = 0;
            auto result = tree.map([&](int value) {
                ++calls;
                return value * p.multiplier + p.offset;
            });
            expectValues(result, p.expected);
            expect(calls, p.expectedCalls, "Число вызовов функции");
            result.clear();
            expectValues(tree, p.input);
        });
    }

    struct WhereCase {
        std::vector<int> input;
        int threshold;
        std::vector<int> expected;
        int expectedCalls;
    };
    const std::vector<WhereCase> whereCases = {
        {{8, 3, 10, 1, 6}, 5, {8, 10, 6}, 5},
        {{8, 3, 10}, 0, {8, 3, 10}, 3},
        {{8, 3, 10}, 100, {}, 3},
        {{}, 5, {}, 0}
    };
    for (std::size_t i = 0; i < whereCases.size(); ++i) {
        const auto& p = whereCases[i];
        tests.run("where #" + show(i + 1), "BinarySearchTree where(Predicate lambda) const", [&] {
            const auto tree = makeTree(p.input);
            int calls = 0;
            auto result = tree.where([&](int value) { ++calls; return value > p.threshold; });
            expectValues(result, p.expected);
            expect(calls, p.expectedCalls, "Число вызовов предиката");
            result.clear();
            expectValues(tree, p.input);
        });
    }

    struct ReduceCase { std::vector<int> input; int start; int expected; };
    const std::vector<ReduceCase> reduceCases = {
        {{8, 3, 10, 1, 6}, 100, 128}, {{}, 42, 42}
    };
    for (std::size_t i = 0; i < reduceCases.size(); ++i) {
        const auto& p = reduceCases[i];
        tests.run("reduce: сумма #" + show(i + 1),
                  "Accumulator reduce(Accumulator start, func lambda) const", [&] {
            const auto tree = makeTree(p.input);
            expect(tree.reduce(p.start, [](int sum, int value) { return sum + value; }),
                   p.expected, "Сумма");
        });
    }

    tests.run("reduce: порядок обхода и строковый аккумулятор",
              "Accumulator reduce(Accumulator start, func lambda) const", [] {
        const std::vector<int> input = {8, 3, 10, 1, 6};
        const std::string start = "";
        const std::string expected = "8,3,1,6,10,";
        const auto tree = makeTree(input);
        expect(tree.reduce(start, [](std::string text, int value) {
            return text + show(value) + ',';
        }), expected, "Порядок обхода");
    });

    tests.run("Конструктор с компаратором", "explicit BinarySearchTree(Compare compare)", [] {
        const std::vector<int> input = {8, 3, 10, 1, 6};
        const bool descending = true;
        const std::string expectedOrder = "8,10,3,6,1,";
        BinarySearchTree<int, Order> tree(Order{descending});
        for (int value : input) tree.insert(value);
        const auto order = [](const auto& t) {
            return t.reduce(std::string{}, [](std::string text, int value) {
                return text + show(value) + ',';
            });
        };
        expect(order(tree), expectedOrder, "Обход с компаратором");
        auto mapped = tree.map([](int value) { return value; });
        auto filtered = tree.where([](int) { return true; });
        expect(order(mapped), expectedOrder, "Компаратор после map");
        expect(order(filtered), expectedOrder, "Компаратор после where");
        BinarySearchTree<int, Order> moved(std::move(tree));
        BinarySearchTree<int, Order> assigned(Order{false});
        assigned = std::move(moved);
        expect(order(assigned), expectedOrder, "Компаратор после перемещения");
        for (int value : input) expect(assigned.erase(value), true, "Удаление с компаратором");
        expectValues(assigned, {});
    });

    tests.run("Освобождение объектов при clear", "void clear() noexcept", [] {
        const std::vector<int> input = {8, 3, 10};
        const int expectedBeforeClear = 3;
        const int expectedAfterClear = 0;
        BinarySearchTree<Tracked> tree;
        for (int value : input) tree.insert(Tracked{value});
        expect(Tracked::alive, expectedBeforeClear, "Живые объекты до clear");
        tree.clear();
        expect(Tracked::alive, expectedAfterClear, "Живые объекты после clear");
    });

    tests.run("Деструктор", "~BinarySearchTree()", [] {
        const std::vector<int> input = {8, 3, 10};
        const int expectedAfterDestruction = 0;
        {
            BinarySearchTree<Tracked> tree;
            for (int value : input) tree.insert(Tracked{value});
        }
        expect(Tracked::alive, expectedAfterDestruction, "Живые объекты после деструктора");
    });

    tests.print();
}

#endif // LAB1_BINARY_SEARCH_TREE_TESTS_H
