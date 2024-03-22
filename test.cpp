#include <iostream>
#include "Localized.hpp"

struct Something{
    void init(){}
};

int main(void){
    constexpr unsigned int MAX=256;
    Localized<Something, MAX> memory;

    std::cout<<std::boolalpha<<memory.isEmpty()<<'\n';
    for(int i=0; i<MAX+2; ++i){
        auto id=memory.allocate(true);
        if(not id.has_value()){
            std::cout<<"cannot alloc "<<i<<'\n';
        }
    }

    std::cout<<std::boolalpha<<memory.isFull()<<'\n';

    memory.free(122);
    memory.free(80);
    memory.free(80);
    memory.free(17);
    memory.free(76);

    for(int i=0; i<6; ++i){
        auto id=memory.allocate();
        if(id.has_value()){
            std::cout<<id.value()<<'\n';
        } else{
            std::cout<<"cannot alloc\n";
        }
    }

}