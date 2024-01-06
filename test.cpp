#include <concepts>
#include <functional>
#include <iostream>
#include <memory>

struct Actor {
    int i = 36;
};

template <class Base, class Owner>
struct Makable {
    template <class Derived>
        requires std::derived_from<Derived, Base>
    static auto make(std::shared_ptr<Owner> owner) noexcept {
        struct ctor_proxy : public Derived {
            ctor_proxy(std::shared_ptr<Actor> owner) noexcept : Derived(owner) {}
        };
        std::shared_ptr<Derived> derived = std::make_shared<ctor_proxy>(owner);
        std::static_pointer_cast<Makable>(derived)->postConstruct(derived);
        return derived;
    }

  private:
    virtual void postConstruct(std::shared_ptr<Base> self) noexcept = 0;
};

struct Component : public Makable<Component, Actor> {
    std::weak_ptr<Actor> owner;

  protected:
    Component(std::shared_ptr<Actor> owner) noexcept : owner(owner) {}
    virtual void postConstruct(std::shared_ptr<Component> self) noexcept override {
        std::cout << "CALL BASE" << std::endl;
    }
};

struct AComponent : public Component {
    int i = 42;

  protected:
    AComponent(std::shared_ptr<Actor> owner) noexcept : Component(owner) {}
    void postConstruct(std::shared_ptr<Component> self) noexcept override {
        std::cout << "CALL DERIVED" << std::endl;
    }
};

int main(void) {
    auto a = std::make_shared<Actor>();
    auto c = Component::make<AComponent>(a);

    std::cout << c->i << c->owner.lock()->i << std::endl;
}