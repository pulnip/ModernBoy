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