#pragma once

#include <cstdint>
#include <map>

#include "Observer.hpp"
#include "Observable.hpp"
#include "SubEngine.hpp"
#include "gefwd.hpp"

struct Key {
    enum class Status {
        PRESSED,
        RELEASED
    } status;
    uint8_t key;
};

class InputSystem : public SubEngine, public Observable<GameStatus> {
  public:
    void registerKey(
        const uint8_t key,
        const std::weak_ptr<Observer<Key>> subscriber
    ) noexcept;

  protected:
    InputSystem() noexcept=default;
    virtual void postConstruct() noexcept override = 0;

  private:
    virtual void update(const float& deltaTime) noexcept override = 0;

  protected:
    std::map<uint8_t, Observable<Key>> keyMap;
};

class NullInputSystem: public InputSystem{
  private:
    void postConstruct() noexcept override final{}
    void update(const float&) noexcept override final{}
};

class InputSystem_default: public InputSystem{
  private:
    void update(const float& deltaTime) noexcept override final;
};
