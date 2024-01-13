#pragma once

#include <cstdint>
#include <functional>
#include <map>

#include "Component.hpp"

class InputComponent : public Component {
  public:
    void processInput(const uint8_t *keyState) noexcept override {
        inputResult = keyState;
    }
    virtual void update(const float &deltaTime) noexcept override;

    ComponentName getName() const noexcept override {
        return ComponentName::InputComponent;
    }
    void setKey(const uint8_t key, std::function<void(void)> behavior) {
        keymap[key] = behavior;
    }
    void setIfNotKey(std::function<void(void)> behavior) {
        behaviorInStandBy = behavior;
    }

  protected:
    InputComponent(const std::weak_ptr<Actor> owner) noexcept
        : Component(owner) {
        updateOrder = 100;
    }

  protected:
    std::map<uint8_t, std::function<void(void)>> keymap;
    std::function<void(void)> behaviorInStandBy = []() {};

  private:
    const uint8_t *inputResult;
};
