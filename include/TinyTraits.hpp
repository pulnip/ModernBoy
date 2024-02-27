#pragma once

#include <memory>
#include "myfwd.hpp"

template<typename Base>
class Singleton{
  public:
    static auto get() noexcept{ return base; }

  protected:
public:
    static std::shared_ptr<Base> base;
};

template<typename T>
extern std::shared_ptr<T> Singleton<T>::base;

class Connectable{
  public:
    virtual void connect() noexcept=0;
};