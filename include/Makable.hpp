#pragma once

#include <concepts>
#include <memory>

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
