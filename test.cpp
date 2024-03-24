#include <iostream>
#include "Localized.hpp"



struct Something{
    void init(){}
    virtual void func(){
        std::cout<<id<<" Do Something...\n";
    }

    static int seed;
    int id=seed++;
};

struct ComplexThing: public Something{
    void func(){
        std::cout<<id<<" Do Complex thing...\n";
    }
};

int Something::seed=0;

int main(void){
    constexpr unsigned int MAX=256;
    Localized<ComplexThing, MAX> memory;

    auto result=memory.allocate();
    if(result.has_value()){
        auto a=result.value();

        a->func();
        a.free();
    }
}