#pragma once

#include <memory>
#include "System/Logging/Bind.hpp"
#include "System/Time/Timer.hpp"

class Core{
  public:
    Core(std::unique_ptr<Timer>) noexcept;
    virtual ~Core()=default;

    void run() noexcept;
    void stop() noexcept{
        isRunning=false;
    }

  private:
    bool isRunning=false;
    Logging::Bind logger={"Core", "base"};
    std::unique_ptr<Timer> timer;
};
