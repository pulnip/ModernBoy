#pragma once

#include "Observer.hpp"
#include "SubEngine.hpp"

enum class GameStatus;

class GameLogic : public SubEngine, public Observer<GameStatus> {
  public:
    virtual ~GameLogic()=default;
    virtual void update(const float &deltaTime) noexcept override=0;
    
    bool isReady() const noexcept { return ready; }

  protected:
    GameLogic() noexcept=default;

  private:
    void onNotify(GameStatus status) noexcept override final;
    SubEngineName getName() const noexcept override final{
        return SubEngineName::GameLogic;
    }

  private:
    virtual void injectDependency() noexcept override=0;
  
  private:
    bool ready = true;
};

class NullGameLogic: public GameLogic{
private:
    void update(const float &deltaTime) noexcept override final{}
    void injectDependency() noexcept override final{}
};
