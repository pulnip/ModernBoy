#include <iostream>

#include "Property.hpp"

struct A {
    A() = default;

    A(const A &) = default;
    void operator=(const A &) {}
};

int main(void) {
    auto ua = std::unique_ptr<A>();
    auto ua2 = std::move(ua);

    Property<A> pa = A();
    Property<A> pa2 = std::move(pa);

    int i = 0;
    Property<int> p1 = i;
    Property<int> p2 = 42;
    auto p3 = p1;
    auto p4 = Property<int>(0);

    p1();
    p1(i);
    p1(34);

    int raw_i = p1;

    p1 = i;
    p1 = 42;
    p1 = p3;
    p1 = Property<int>(0);
}