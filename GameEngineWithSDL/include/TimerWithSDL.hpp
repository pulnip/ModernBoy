#pragma once

#include "SubEngine/Timer.hpp"

namespace WithSDL{
    namespace SubEngine{
        class Timer final: public Game::SubEngine::Timer{
        private:
            using time_point = uint64_t;

        public:
            Timer();
            ~Timer()=default;

        private:
            void reset() noexcept override final;
            void wait(const millisecond timeout) noexcept override final;
            millisecond getDeltaTime() noexcept override final;

        private:
            time_point lastTimePoint;
        };
    }
}
