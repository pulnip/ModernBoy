#include <thread>
#include "WithSTD/Timer.hpp"

void WithSTD::Timer::reset() noexcept{
    lastTimePoint = clock.now();
}

void WithSTD::Timer::wait(const unit timeout) noexcept {
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
}

Timer::unit WithSTD::Timer::getDeltaTime() noexcept {
    const auto now = clock.now();
    const auto delta = now - lastTimePoint;

    lastTimePoint = now;

    return duration_cast<milliseconds>(delta).count();
}