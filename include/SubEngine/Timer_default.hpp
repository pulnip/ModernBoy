#pragma once

#include <chrono>

#include "Timer.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;

class Timer_default : public Timer {
  public:
    Timer_default() = default;

  private:
    void injectDependency() noexcept override final{}
    void reset() noexcept override final;
    void wait(const millisecond timeout) noexcept override final;
    millisecond getDeltaTime() noexcept override final;

  private:
    high_resolution_clock clock;
    high_resolution_clock::time_point lastTimePoint = clock.now();
};