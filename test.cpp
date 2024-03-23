#include <iostream>
#include "Localized.hpp"



struct Something{
    void init(){}
    void func(){
        std::cout<<id<<" Do Something...\n";
    }

    static int seed;
    int id=seed++;
};

int Something::seed=0;

int main(void){
    constexpr unsigned int MAX=256;
    Localized<Something, MAX> memory;
    {
        auto pSome=memory.allocate().value();
        pSome->func();
        (*pSome).func();
        pSome.free();

    }

    (*memory.allocate().value()).func();
}