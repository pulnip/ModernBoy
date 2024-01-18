#pragma once

#include "Observable.hpp"
#include "SubEngine.hpp"

enum class GameStatus;

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
    void onNotify(Key key) noexcept override final;

  protected:
    virtual void injectDependency() noexcept override;

  private:
    virtual void update(const float &deltaTime) noexcept override=0;
};

class NullGameLogic: public GameLogic{
private:
    void update(const float &deltaTime) noexcept override final{}
    void injectDependency() noexcept override final{
        GameLogic::injectDependency();
    }
};
