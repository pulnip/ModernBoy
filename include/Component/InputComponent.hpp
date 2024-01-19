#pragma once

#include <cstdint>
#include <functional>
#include <map>

#include "Component.hpp"

class InputComponent: public Component,
    public Observer<Key>
{
  public:
    virtual ~InputComponent()=default;

    void setKey(
        const uint8_t key,
        std::function<void(void)> OnPressed,
        std::function<void(void)> OnReleased=[](){}
    ) noexcept;

  protected:
    InputComponent() noexcept=default;

    void injectDependency() noexcept override final;

  private:
    void update(const float &deltaTime) noexcept override final{}
    ComponentName getName() const noexcept override final{
        return ComponentName::InputComponent;
    }
    int initUpdateOrder() const noexcept override final{ return 100; }

    void onNotify(Key key) noexcept override final;

  protected:
    std::map<uint8_t, std::function<void(void)>> ifPressed;
    std::map<uint8_t, std::function<void(void)>> ifReleased;

  private:
    std::weak_ptr<InputSystem> inputSystem;
};
