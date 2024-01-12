#pragma once

#include "Observer.hpp"
#include "SubEngine.hpp"

enum class GameStatus;

class GameLogic : public SubEngine, public Observer<GameStatus> {
  public:
    void update(const float &deltaTime) noexcept override;
    bool isReady() const noexcept { return ready; }

  protected:
    GameLogic(const OwnerRef owner) noexcept : SubEngine(owner) {}

  private:
    void postConstruct() noexcept override;
    void onNotify(GameStatus status) noexcept override;

  private:
    bool ready = true;
};
