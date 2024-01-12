#pragma once

#include <cstdint>
#include <map>

#include "Observable.hpp"
#include "SubEngine.hpp"

enum class GameStatus;

struct Key {
    enum class Status {
        PRESSED,
        RELEASED
    } status;
    uint8_t key;
};

class InputSystem : public SubEngine, public Observable<GameStatus> {
  public:
    virtual void update(const float &deltaTime) noexcept override = 0;

  protected:
    InputSystem(const OwnerRef owner) noexcept : SubEngine(owner) {}
    virtual void postConstruct() noexcept override = 0;

  protected:
    std::map<uint8_t, Observable<Key>> keyMap;
};
