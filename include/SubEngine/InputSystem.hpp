#pragma once

#include <cstdint>
#include <map>

#include "Observable.hpp"
#include "SubEngine.hpp"

struct Key {
    enum class Status {
        PRESSED,
        RELEASED
    } status;
    uint8_t key;
};

class InputSystem: public SubEngine{
  public:
    virtual ~InputSystem()=default;

    void registerKey(
        const uint8_t key,
        const std::weak_ptr<Observer<Key>> subscriber
    ) noexcept;

  protected:
    InputSystem() noexcept=default;

  private:
    SubEngineName getName() const noexcept override final{
        return SubEngineName::InputSystem;
    }
    virtual void injectDependency() noexcept override final;

  private:
    virtual void update(const float& deltaTime) noexcept override=0;
    virtual void setAttribute() noexcept=0;

    virtual uint8_t escapeKeycode() const noexcept{ return 0x1b; }

  protected:
    std::map<uint8_t, Observable<Key>> keyMap;
};

class NullInputSystem final: public InputSystem{
  private:
    void update(const float&) noexcept override final{}
    void setAttribute() noexcept final{}
};

// for std input
class InputSystem_default final: public InputSystem{
  private:
    void update(const float& deltaTime) noexcept override final;
    void setAttribute() noexcept final{}

    uint8_t escapeKeycode() const noexcept override final{ return 'q'; }
};
