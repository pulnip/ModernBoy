#include <iostream>

#include "Makable.hpp"

class Foo : public Makable<Foo> {
    void postConstruct() noexcept override = 0;
};

class Bar : public Foo {
    void postConstruct() noexcept override {
        std::cout << "call" << std::endl;
    }
};

int main(void) {
    auto foo = Foo::make<Bar>();
}