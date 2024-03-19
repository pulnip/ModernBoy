#pragma once

#include <memory>
#include "Concept.hpp"
#include "Makable.hpp"

template<typename Base>
class Singleton{
  public:
    static auto get() noexcept{ return base; }

  protected:
    static std::shared_ptr<Base> base;
};

template<typename Base>
extern std::shared_ptr<Base> Singleton<Base>::base;

template<typename Base, bool hasPreCondition, bool hasPostCondition>
class MakableSingleton:
    public Singleton<Base>, public Makable<Base, hasPreCondition, hasPostCondition>{
  public:
    template<typename Derived, typename... T>
    static void make(T... args) noexcept{
        Singleton<Base>::base=Makable<Base,
            hasPreCondition, hasPostCondition
        >::template make<Derived>(args...);
    }
};