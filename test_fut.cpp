#include <future>
#include <memory>
#include <iostream>
#include <thread>

using namespace std::literals::chrono_literals;

struct Data {
    void doSomething() {
        std::cout << "DO SOMETHING\n";
    }
};

struct Owner {
    std::promise<std::shared_ptr<Data>> promise;
    std::shared_future<std::shared_ptr<Data>> future;

    Owner() : future(promise.get_future()) {}

    void init(std::shared_ptr<Data> newData){
        promise.set_value(newData);
    }

    std::shared_future<std::shared_ptr<Data>> getData(){
        return future;
    }
};

Owner owner;

struct Other{
    Other(){
        owner.getData().get()->doSomething();
    }
};

int main(int argc, char* argv[]) {
    auto task1=std::async(std::make_shared<Other>);
    auto task2=std::async(std::make_shared<Other>);

    std::this_thread::sleep_for(1s);

    owner.init(std::make_shared<Data>());

    auto task3=std::async(std::make_shared<Other>);

    auto other1=task1.get();
    auto other2=task2.get();
    auto other3=task3.get();

    return 0;
}
