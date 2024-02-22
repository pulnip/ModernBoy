#include <SDL2/SDL_timer.h>
#include "Engine/Logger.hpp"
#include "WithSDL/Timer.hpp"

using namespace WithSDL;

Timer::Timer() noexcept:
    logger(std::make_unique<Engine::BindedLogger>("Timer", "SDL")),
    lastTimePoint(SDL_GetTicks64())
{
    logger->debug("constructed");
}

Timer::~Timer(){
    logger->debug("destructed");
}


void Timer::reset() noexcept {
    lastTimePoint = SDL_GetTicks64();
}

void Timer::wait(const millisecond timeout) noexcept {
    const auto timeoutTimePoint = lastTimePoint + timeout;
    // equivalent to using SDL_TICKS_PASSED ( <- 32bits ver.)
    while (SDL_GetTicks64() <= timeoutTimePoint);
}

Timer::millisecond Timer::getDeltaTime() noexcept {
    const auto timeEntered = SDL_GetTicks64();
    const auto delta = timeEntered - lastTimePoint;

    lastTimePoint = timeEntered;

    return delta;
}