#pragma once

#include <memory>
#include "Concept.hpp"
#include "myfwd.hpp"

template<typename Base>
class Singleton{
  public:
    static auto get() noexcept{ return base; }

  protected:
    static std::shared_ptr<Base> base;
};

template<typename Base>
extern std::shared_ptr<Base> Singleton<Base>::base;

class Connectable{
  public:
    virtual void connect() noexcept=0;
};

template<typename Base>
class Makable{
  public:
    template<typename Derived, typename... T>
        requires std::derived_from<Derived, Base>
    static std::shared_ptr<Derived> make(T... args) noexcept{
        static_assert(std::derived_from<Base, Makable>);
        Derived::preConstruct();
        auto derived=std::make_shared<Derived>(args...);
        derived->postConstruct();

        return derived;
    }

    virtual void postConstruct() noexcept=0;
};

template<typename Base>
class MakableSingleton:
    public Singleton<Base>, public Makable<Base>{
  public:
    template<typename Derived, typename... T>
    static void make(T... args) noexcept{
        Singleton<Base>::base=Makable<Base>::template make<Derived>(args...);
    }
};