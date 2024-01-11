#include <cassert>
#include <iostream>
#include <map>
#include <optional>

struct A {
    A() {
        std::cout << "ctor" << std::endl;
    }
    operator std::optional<A>() {
        return *this;
    }
};

std::optional<A> func(int i) {
    return i != 0 ? static_cast<std::nullopt_t>(A()) : std::nullopt;
}

int main(void) {
    auto r = func(1);

    if (nullptr) {
        std::cout << "NOT NULL" << std::endl;
    } else {
        std::cout << "NULL" << std::endl;
    }
    // std::map<const int, A *> imap;
    // imap.emplace(1, new A());

    // A *object = new A();

    // auto [it, NotExist] = imap.try_emplace(1, object);

    // assert(!NotExist);

    // // testing...
    // if (object == it->second) {
    //     std::cout << "it is new" << std::endl;
    // } else {
    //     std::cout << "Not Expected" << std::endl;
    // }

    // return 0;
}