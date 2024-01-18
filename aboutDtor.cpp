#include <iostream>

struct Slave{
    Slave(){ std::cout<<"Slave Ctor"<<'\n'; }
    ~Slave(){ std::cout<<"Slave Dtor"<<'\n'; }
};

struct Master{
    Master(){ std::cout<<"Master Ctor"<<'\n'; }
    ~Master(){ std::cout<<"Master Dtor"<<'\n'; }
    Slave slave;
};

int main(void){
    Master master;
}