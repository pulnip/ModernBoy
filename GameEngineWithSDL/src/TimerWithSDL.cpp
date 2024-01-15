#include <SDL2/SDL_timer.h>

#include "TimerWithSDL.hpp"

TimerWithSDL::TimerWithSDL() : lastTimePoint(SDL_GetTicks64()) {}

void TimerWithSDL::reset() noexcept {
    lastTimePoint = SDL_GetTicks64();
}

void TimerWithSDL::wait(const millisecond timeout) noexcept {
    const auto timeoutTimePoint = lastTimePoint + timeout;
    // equivalent to using SDL_TICKS_PASSED ( <- 32bits ver.)
    while (SDL_GetTicks64() <= timeoutTimePoint)
        ;
}

Timer::millisecond TimerWithSDL::getDeltaTime() noexcept {
    const auto timeEntered = SDL_GetTicks64();
    const auto delta = timeEntered - lastTimePoint;

    lastTimePoint = timeEntered;

    return delta;
}