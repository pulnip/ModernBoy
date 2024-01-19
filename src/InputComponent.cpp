#include <cassert>

#include "SubEngine/InputSystem.hpp"
#include "Actor/IActor.hpp"
#include "Component/InputComponent.hpp"

void InputComponent::setKey(
    const uint8_t key,
    std::function<void(void)> OnPressed,
    std::function<void(void)> OnReleased
) noexcept{
    assert(!inputSystem.expired());
    inputSystem.lock()->registerKey(
        key, std::dynamic_pointer_cast<InputComponent>(shared_from_this())
    );

    ifPressed[key] = OnPressed;
    ifReleased[key] = OnReleased;
}

void InputComponent::injectDependency() noexcept{
    assert(!owner.expired());
    auto query=owner.lock()->query(SubEngineName::InputSystem);
    
    assert(query.has_value());
    inputSystem=std::dynamic_pointer_cast<InputSystem>(query.value());
}

void InputComponent::onNotify(Key key) noexcept {
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


