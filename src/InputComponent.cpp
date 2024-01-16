#include <cassert>

#include "Component/InputComponent.hpp"
#include "SubEngine/InputSystem.hpp"

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

void InputComponent::setKey(
    const uint8_t key,
    std::function<void(void)> OnPressed,
    std::function<void(void)> OnReleased
) noexcept{
    ifPressed[key] = OnPressed;
    ifReleased[key] = OnReleased;
}

