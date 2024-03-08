#include <iostream>
#include <memory>
#include <vector>

class Callable{
  public:
    virtual void operator()()=0;
};;

class Predicate{
  public:
    virtual bool operator()()=0;
};

class Foo{
  public:
    void func(void){
        std::cout<<"FUNC CALL\n";
    }
};

class SomeChecker: public Predicate{
  public:
    bool operator()() noexcept override{
        return id & 0x01;
    }
  private:
    static int seed;
    int id=++seed;
};

int SomeChecker::seed=10;

class Caller: public Callable{
  public:
    void operator()() noexcept override{
        foo.func();
    }

  private:
    Foo foo;
};


int main(void){
    std::vector<std::shared_ptr<Callable>> callables;
    std::vector<std::shared_ptr<Predicate>> predicates;

    for(int i=0; i<10; ++i){
        callables.emplace_back(std::make_shared<Caller>());
        predicates.emplace_back(std::make_shared<SomeChecker>());
    }

    for(auto& c: callables){
        (*c)();
    }

    for(auto& p: predicates){
        std::cout<<std::boolalpha<<(*p)()<<'\n';
    }


    return 0;
}