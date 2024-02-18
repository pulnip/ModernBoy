#pragma once

#include <cstdint>
#include <chrono>

#include "Makable.hpp"

namespace Game{
    namespace SubEngine{
        class Timer: public Makable<Timer>{
        public:
            using millisecond = uint64_t;

            Timer() noexcept=default;
            virtual ~Timer()=default;

            virtual void reset() noexcept=0;
            virtual void wait(const millisecond timeout) noexcept=0;
            virtual millisecond getDeltaTime() noexcept=0;

        private:
            void postConstruct() noexcept override final{}
        };
    }
}

namespace WithSTD{
    namespace SubEngine{
        using namespace std::chrono;
        using namespace std::chrono_literals;

        class Timer_default final: public Game::SubEngine::Timer{
        public:
            Timer_default()=default;
            ~Timer_default()=default;

        private:
            void reset() noexcept override final;
            void wait(const millisecond timeout) noexcept override final;
            millisecond getDeltaTime() noexcept override final;

        private:
            high_resolution_clock clock;
            high_resolution_clock::time_point lastTimePoint = clock.now();
        };
    }
}