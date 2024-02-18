#include <cassert>

#include "SubEngine/InputSystem.hpp"
#include "Actor/Actor.hpp"
#include "Component/Controllable.hpp"

void Controllable::setKey(
    const uint8_t key,
    std::function<void(void)> OnPressed,
    std::function<void(void)> OnReleased
) noexcept{
    assert(!inputSystem.expired());
    inputSystem.lock()->registerKey(
        key, std::static_pointer_cast<Controllable>(shared_from_this())
    );

    ifPressed[key] = OnPressed;
    ifReleased[key] = OnReleased;
}

void Controllable::injectDependency() noexcept{
    assert(!owner.expired());
    std::shared_ptr<Actor::Interface> actor=owner.lock();
    auto query=actor->query(SubEngine::Type::InputSystem);
    
    assert(query.has_value());
    inputSystem=std::dynamic_pointer_cast<InputSystem>(query.value());
}

void Controllable::onNotify(Key key) noexcept {
    if(key.status==Key::Status::PRESSED){
        auto it=ifPressed.find(key.key);
        assert(it != ifPressed.end());
        it->second();
    } else{
        auto it=ifReleased.find(key.key);
        assert(it != ifReleased.end());
        it->second();
    }
}
