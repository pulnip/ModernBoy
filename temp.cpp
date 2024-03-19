#include <memory>
#include <cassert>

template<typename T>
class Mediator{
  public:
    static T& get(){
        assert(not t.expired());
        return *t.lock();
    }
    static void set(const std::shared_ptr<T>& sp){
        Mediator<T>::t = sp;
    }

  private:
    static std::weak_ptr<T> t;
};

template<typename T>
std::weak_ptr<T> Mediator<T>::t;

class NullSystem;
class System{};
class Impl: public System{};

class Main{
  public:
    Main()
    :sys(std::make_shared<Impl>()){
        Mediator<System>::set(sys);
    }

    std::shared_ptr<System> sys;
};


int main(void){
    Main m;

    auto i=Mediator<System>::get();
}