#include <thread>
#include "Engine/Timer.hpp"
#include "Engine/Logger.hpp"

using namespace WithSTD;

Timer::Timer() noexcept:
    logger(std::make_unique<Engine::BindedLogger>("Timer", "STD"))
{
    logger->debug("constructed");
}

Timer::~Timer(){
    logger->debug("destructed");
}

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