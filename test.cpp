#include <memory>
#include <iostream>

class Base{
    virtual void func()=0;
};

class Derived: public Base{
public:
    void func() noexcept override{}
};

int main(void){
    auto d=std::make_shared<Derived>();
    std::weak_ptr<Base> wb=d;

    std::weak_ptr<Derived> new_wd=std::dynamic_pointer_cast<Derived>(wb.lock());
    std::cout<<new_wd.lock().use_count()<<std::endl;
}