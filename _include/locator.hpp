#pragma once

#include <memory>

template<typename T>
class Locator{
  public:
    static std::shared_ptr<T> get();
    static void provide(std::shared_ptr<T>);

  private:
    static std::weak_ptr<T> service;
};

template<typename T>
std::shared_ptr<T> Locator<T>::get(){
    if(service.expired()){
        service=T::null;
    }
    return service.lock();
}

template<typename T>
void Locator<T>::provide(std::shared_ptr<T> ptr){
    if(ptr!=nullptr){
        service=ptr;
    } else{
        service=T::null;
    }
}