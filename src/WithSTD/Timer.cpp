#include <thread>
#include "Engine/Timer.hpp"
#include "Engine/Logger.hpp"

using namespace WithSTD;

void Timer::reset() noexcept{
    lastTimePoint = clock.now();
}

void Timer::wait(const millisecond timeout) noexcept {
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
}

Timer::millisecond Timer::getDeltaTime() noexcept {
    const auto now = clock.now();
    const auto delta = now - lastTimePoint;

    lastTimePoint = now;

    return duration_cast<milliseconds>(delta).count();
}