class Base{
    virtual void func()=0;
};

class Derived: public Base{
public:
    void func() noexcept override{}
};

int main(void){
    Derived d;

    d.func();
}