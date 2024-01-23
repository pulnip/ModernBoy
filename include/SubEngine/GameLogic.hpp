#pragma once

#include "SubEngine.hpp"

enum class GameStatus {
    GAME_OVER,
    FORCE_QUIT,
    UNEXPECTED
};

class GameLogic: public SubEngine,
    public Observable<GameStatus>,
    public Observer<Key>
{
  public:
    virtual ~GameLogic()=default;

  protected:
    GameLogic() noexcept=default;

  private:
    SubEngineName getName() const noexcept override final{
        return SubEngineName::GameLogic;
    }
    virtual void injectDependency() noexcept override final;

    void onNotify(Key key) noexcept override final;

  private:
    virtual void update(const float &deltaTime) noexcept override=0;
};

class NullGameLogic: public GameLogic{
  private:
    void update(const float &deltaTime) noexcept override final{}
};
