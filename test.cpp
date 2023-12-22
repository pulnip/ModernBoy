#include <string>
#include <typeinfo>
#include <ranges>
#include <map>
#include <iostream>

struct Base{};
struct Derived: Base{};

int main(void){
    Base* pb=new Base();
    Derived* pd1=new Derived();
    Base* pd2=new Derived();

    std::cout<<typeid(pb).name()<<std::endl;
    std::cout<<typeid(pd1).name()<<std::endl;
    std::cout<<typeid(pd2).name()<<std::endl;
}