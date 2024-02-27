#pragma once

#include <cstdint>
#include <chrono>
#include <memory>
#include "TinyTraits.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"

namespace Engine{
    class Timer: public Singleton<Timer>{
      public:
        using millisecond = uint64_t;

        virtual ~Timer()=default;

        virtual void reset() noexcept=0;
        virtual void wait(const millisecond timeout) noexcept=0;
        virtual millisecond getDeltaTime() noexcept=0;

      private:
        ::Logger::Binded logger={"Timer", "base"};
    };
}

namespace WithSTD{
    using namespace std::chrono;
    using namespace std::chrono_literals;

    class Timer final: public Engine::Timer{
      public:
        Timer() noexcept;
        ~Timer();

      private:
        void reset() noexcept override final;
        void wait(const millisecond timeout) noexcept override final;
        millisecond getDeltaTime() noexcept override final;

      private:
        ::Logger::Binded logger={"Timer", "STD"};
        high_resolution_clock clock;
        high_resolution_clock::time_point lastTimePoint = clock.now();
    };
}