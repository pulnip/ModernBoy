#include "TinyTraits.hpp"

class Abstract: public MakableSingleton<Abstract>{
  public:
    void func(void) noexcept{
        // do something...
    }
  private:
    void postConstruct() noexcept override final{
        // do something...
    }
};

class Concrete: public Abstract{};

int main(void){
    Abstract::make<Concrete>();

    Abstract::get()->func();
}