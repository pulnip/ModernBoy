#pragma once

#include "Engine/Timer.hpp"
#include "Engine/Logger.hpp"

namespace WithSDL{
    class Timer final: public Engine::Timer{
    private:
        using time_point = uint64_t;

    public:
        Timer() noexcept;

    private:
        void reset() noexcept override final;
        void wait(const millisecond timeout) noexcept override final;
        millisecond getDeltaTime() noexcept override final;

      private:
        ::Logger::Binded logger={"Timer", "SDL"};
        time_point lastTimePoint;
    };
}
