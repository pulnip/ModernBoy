#include "InputComponent.hpp"

void InputComponent::update(const float &deltaTime) noexcept {
    uint8_t pressedCount = 0;

    for (auto &pair : keymap) {
        if (inputResult[pair.first]) {
            pressedCount += 1;
            pair.second();
        }
    }
    if (pressedCount == 0) {
        behaviorInStandBy();
    }
}
