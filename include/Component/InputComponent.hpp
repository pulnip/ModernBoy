#pragma once

#include <cstdint>
#include <functional>
#include <map>

#include "Observer.hpp"
#include "gefwd.hpp"
#include "Component.hpp"

class InputComponent: public Component,
    public Observer<Key>
{
  public:
    void update(const float &deltaTime) noexcept override final{}
    void onNotify(Key key) noexcept override final;

    ComponentName getName() const noexcept override final{
        return ComponentName::InputComponent;
    }
    void setKey(
        const uint8_t key,
        std::function<void(void)> OnPressed,
        std::function<void(void)> OnReleased=[](){}
    ) noexcept;

  protected:
    InputComponent() noexcept{ updateOrder = 100; }
    // must 
    void injectDependency() noexcept override{}

  protected:
    std::map<uint8_t, std::function<void(void)>> ifPressed;
    std::map<uint8_t, std::function<void(void)>> ifReleased;
};
