#pragma once

#include "Engine/Timer.hpp"

namespace WithSDL{
    class Timer final: public Engine::Timer{
    private:
        using time_point = uint64_t;

    public:
        Timer() noexcept;
        ~Timer();

    private:
        void reset() noexcept override final;
        void wait(const millisecond timeout) noexcept override final;
        millisecond getDeltaTime() noexcept override final;

      private:
        std::unique_ptr<Engine::BindedLogger> logger;
        time_point lastTimePoint;
    };
}
