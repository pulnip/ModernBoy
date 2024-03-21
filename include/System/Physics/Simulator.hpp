#pragma once

#include <memory>
#include "Engine/Types.hpp"
#include "System/Logging/Bind.hpp"
#include "FWD.hpp"

class Simulator{
  protected:
    using ActorID=int;
    using Time=Game::Time;

  public:
    Simulator() noexcept;
    virtual ~Simulator()=default;

    virtual void update(const Time& deltaTime)=0;

    virtual void request(const Actor&)=0;



    bool updateTarget(wp, const Actor::Role&, const Game::Time&) noexcept;
    void updateTarget_aux(sp, const std::vector<wp>&, const Game::Time&) noexcept;

    void append(pActor) noexcept;

  private:
    virtual void redoUpdateIfCollide(
        sp who, sp to, const Game::Time& dt
    ) noexcept=0;

  public:
    static std::shared_ptr<Simulator> null;

  private:
    Logging::Bind logger={"Physics", "base"};
};
