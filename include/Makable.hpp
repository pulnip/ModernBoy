#pragma once

#include <concepts>
#include <memory>

// Makable for Owned Object

template <class Base, class Owner = void>
struct Makable {
    template <class Derived>
        requires std::derived_from<Derived, Base> || std::same_as<Derived, Base>
    static auto make(const std::weak_ptr<Owner> owner) noexcept {
        struct ctor_proxy: public Derived{
            ctor_proxy() noexcept: Derived(){}
        };
        std::shared_ptr<Derived> derived = std::make_shared<ctor_proxy>();
        std::static_pointer_cast<Makable>(derived)->owner=owner;

        std::static_pointer_cast<Makable>(derived)->postConstruct();

        return derived;
    }

  private:
    virtual void postConstruct() noexcept = 0;

  protected:
    std::weak_ptr<Owner> owner;
};

// Makable for Non-Owned Object
template <class Base>
struct Makable<Base, void> {
    template <class Derived>
        requires std::derived_from<Derived, Base> || std::same_as<Derived, Base>
    static auto make() noexcept {
        struct ctor_proxy: public Derived{
            ctor_proxy() noexcept: Derived(){}
        };
        std::shared_ptr<Derived> derived = std::make_shared<Derived>();

        std::static_pointer_cast<Makable>(derived)->postConstruct();
        
        return derived;
    }

  private:
    virtual void postConstruct() noexcept = 0;
};
