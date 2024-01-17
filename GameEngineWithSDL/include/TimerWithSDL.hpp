#pragma once

#include "SubEngine/Timer.hpp"

class TimerWithSDL : public Timer {
  private:
    using time_point = uint64_t;

  public:
    TimerWithSDL();

    void reset() noexcept override;
    void wait(const millisecond timeout) noexcept override;
    millisecond getDeltaTime() noexcept override;

  private:
    time_point lastTimePoint;
};