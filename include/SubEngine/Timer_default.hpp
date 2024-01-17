#pragma once

#include <chrono>

#include "Timer.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;

class Timer_default : public Timer {
  public:
    Timer_default() = default;

    void reset() noexcept override;
    void wait(const millisecond timeout) noexcept override;
    millisecond getDeltaTime() noexcept override;

  private:
    high_resolution_clock clock;
    high_resolution_clock::time_point lastTimePoint = clock.now();
};