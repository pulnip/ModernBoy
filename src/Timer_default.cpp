#include <thread>

#include "Timer_default.hpp"

void Timer_default::reset() noexcept {
    lastTimePoint = clock.now();
}

void Timer_default::wait(const millisecond timeout) noexcept {
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
}

Timer::millisecond Timer_default::getDeltaTime() noexcept {
    const auto now = clock.now();
    const auto delta = now - lastTimePoint;

    lastTimePoint = now;

    return duration_cast<milliseconds>(delta).count();
}