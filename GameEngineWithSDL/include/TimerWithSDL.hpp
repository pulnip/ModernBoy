#pragma once

#include "SubEngine/Timer.hpp"

class TimerWithSDL final: public Timer{
  private:
    using time_point = uint64_t;

  public:
    TimerWithSDL();
    ~TimerWithSDL()=default;

  private:
    void injectDependency() noexcept override final{}
    void reset() noexcept override final;
    void wait(const millisecond timeout) noexcept override final;
    millisecond getDeltaTime() noexcept override final;

  private:
    time_point lastTimePoint;
};