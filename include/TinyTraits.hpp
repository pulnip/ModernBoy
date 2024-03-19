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

template<typename Base, bool hasPreCondition, bool hasPostCondition>
class Makable{
  public:
    template<typename Derived, typename... T>
        requires std::derived_from<Derived, Base>
    static std::shared_ptr<Derived> make(T... args) noexcept{
        static_assert(std::derived_from<Base, Makable>);
        Derived::preConstruct();
        std::shared_ptr<Makable> derived=std::make_shared<Derived>(args...);
        derived->postConstruct();

        return std::static_pointer_cast<Derived>(derived);
    }

    virtual void postConstruct() noexcept=0;
};

template<typename Base>
class Makable<Base, false, true>{
  public:
    template<typename Derived, typename... T>
        requires std::derived_from<Derived, Base>
    static std::shared_ptr<Derived> make(T... args) noexcept{
        static_assert(std::derived_from<Base, Makable>);
        std::shared_ptr<Makable> derived=std::make_shared<Derived>(args...);
        derived->postConstruct();

        return std::static_pointer_cast<Derived>(derived);
    }

    virtual void postConstruct() noexcept=0;
};

template<typename Base>
class Makable<Base, true, false>{
  public:
    template<typename Derived, typename... T>
        requires std::derived_from<Derived, Base>
    static std::shared_ptr<Derived> make(T... args) noexcept{
        static_assert(std::derived_from<Base, Makable>);
        Derived::preConstruct();
        return std::make_shared<Derived>(args...);
    }
};

template<typename Base>
class Makable<Base, false, false>{
  public:
    template<typename Derived, typename... T>
        requires std::derived_from<Derived, Base>
    static std::shared_ptr<Derived> make(T... args) noexcept{
        static_assert(std::derived_from<Base, Makable>);
        return std::make_shared<Derived>(args...);
    }
};

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

class Callable{
  public:
    virtual void operator()() noexcept=0;
};

class DoNothing final: public Callable{
  public:
    void operator()() noexcept override{}
};

extern std::shared_ptr<DoNothing> doNothing;

class Predicate{
  public:
    virtual bool operator()() noexcept=0;
};

class AlwaysTrue: public Predicate{
  public:
    bool operator()() noexcept override{ return true; }
};

extern std::shared_ptr<AlwaysTrue> alwaysTrue;