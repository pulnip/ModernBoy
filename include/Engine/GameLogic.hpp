#pragma once

#include <memory>
#include "Engine/Types.hpp"
#include "System/Logging/Bind.hpp"

class GameLogic{
    using Time=Game::Time;
    using Status=Game::Status;

  public:
    virtual ~GameLogic()=default;

    void update(const Time& deltaTime) noexcept;
    
    virtual void setStatus(const Status& st) noexcept;

  private:
    Logging::Bind logger={"GameLogic", "base"};
};
