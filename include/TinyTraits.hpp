#pragma once

#include <memory>

template<typename Base>
class Singleton{
  public:
    static auto get() noexcept{ return base; }

  protected:
    static std::shared_ptr<Base> base;
};

template<typename T>
std::shared_ptr<T> Singleton<T>::base;

class Connectable{
  public:
    virtual void connect() noexcept=0;
};