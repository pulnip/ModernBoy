#include <cassert>

#include "Skin.hpp"
#include "Engine/InputSystem.hpp"
#include "Actor/Vanilla.hpp"
#include "Component/Controllable.hpp"

using namespace Component;

Controllable::Controllable(std::weak_ptr<Actor::Vanilla> actor
) noexcept: Ability(actor, 100){}

void Controllable::setKey(
    const uint8_t key,
    std::function<void(void)> OnPressed,
    std::function<void(void)> OnReleased
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
        it->second();
    } else{
        auto it=ifReleased.find(key.key);
        assert(it != ifReleased.end());
        it->second();
    }
}
