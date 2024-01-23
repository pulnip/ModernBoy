#pragma once

#include <concepts>
#include <memory>

// Makable for Owned Object

template <class Base, class Owner = void>
class Makable{
  public:
    template <class Derived>
        requires std::derived_from<Derived, Base>
    static auto make(const std::weak_ptr<Owner> owner) noexcept{
        static_assert(std::derived_from<Base, Makable<Base, Owner>>);

        std::shared_ptr<Makable> derived = std::make_shared<Derived>();

        derived->owner=owner;
        derived->postConstruct();

        return std::static_pointer_cast<Derived>(derived);
    }

    const std::weak_ptr<Owner>& getOwner() const noexcept{ return owner; }

  private:
    virtual void postConstruct() noexcept=0;

  protected:
    std::weak_ptr<Owner> owner;
};

// Makable for Non-Owned Object
template <class Base>
class Makable<Base, void>{
  public:
    template <class Derived>
        requires std::derived_from<Derived, Base>
    static auto make() noexcept{
        static_assert(std::derived_from<Base, Makable<Base>>);

        std::shared_ptr<Makable> derived = std::make_shared<Derived>();

        derived->postConstruct();
        
        return std::static_pointer_cast<Derived>(derived);
    }

  private:
    virtual void postConstruct() noexcept = 0;
};
