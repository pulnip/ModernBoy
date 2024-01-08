#include <set>

struct A {
    int id, order_hint;
};

// bool operator==(const A &lhs, const A &rhs) {
//     return lhs.id == rhs.id;
// }

struct comp_order_hint {
    bool operator()(const A &lhs, const A &rhs) const {
        return lhs.order_hint < rhs.order_hint;
    }
};

#include <iostream>

int main(void) {
    std::multiset<A, comp_order_hint> myset;

    for (int i = 0; i < 10; ++i) {
        myset.emplace(A{i, ((9 - i) / 2) * 100});
    }

    for (auto &i : myset) {
        std::cout << i.id << i.order_hint << std::endl;
    }
}