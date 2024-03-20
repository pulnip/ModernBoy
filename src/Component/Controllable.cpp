#include <cassert>

#include "Skin.hpp"
#include "Engine/InputSystem.hpp"
#include "Actor/Vanilla.hpp"
#include "Component/Controllable.hpp"
#include "Component/Movable.hpp"

using namespace Component;

Controllable::Controllable(std::weak_ptr<Actor::Vanilla> actor
) noexcept: Ability(actor, 100){}

void Controllable::setKey(
    const uint8_t key,
    std::shared_ptr<Callable> OnPressed,
    std::shared_ptr<Callable> OnReleased
) noexcept{
    Engine::InputSystem::get()->registerKey(
        key, std::static_pointer_cast<Controllable>(shared_from_this())
    );

    ifPressed[key] = OnPressed;
    ifReleased[key] = OnReleased;
}

void Controllable::onNotify(const Skin::Key& key) noexcept {
    if(key.status==Skin::Key::Status::PRESSED){
        auto it=ifPressed.find(key.key);
        assert(it != ifPressed.end());
        (*it->second)();
    } else{
        auto it=ifReleased.find(key.key);
        assert(it != ifReleased.end());
        (*it->second)();
    }
}

using namespace Component::Behaviour;

template<typename TargetComponent>
std::optional<std::shared_ptr<TargetComponent>>
Base<TargetComponent>::getTarget() noexcept{
    if(target.expired()){
        connect();
    }

    if(target.expired()){
        return std::nullopt;
    }

    return target.lock();
}

template<typename TargetComponent>
void Base<TargetComponent>::connect() noexcept{
    if(actor.expired()) return;

#warning "change to typeid"
    auto found=actor.lock()->get(Type::Movable);
    if(found){
        target=std::static_pointer_cast<TargetComponent>(found.value());
    }
}

void MoveX::operator()() noexcept{
    auto target=getTarget();

    if(target){
        target.value()->get().velocity.linear.x = vx;
    }
}
void MoveY::operator()() noexcept{
    auto target=getTarget();

    if(target){
        target.value()->get().velocity.linear.y = vy;
    }
}
