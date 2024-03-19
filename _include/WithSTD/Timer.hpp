#include <chrono>
#include "System/Time/Timer.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;

namespace WithSTD{
    class Timer: public ::Timer{
      public:
        Timer() noexcept;
        ~Timer();
    
      private:
        void reset() noexcept override final;
        void wait(const unit timeout) noexcept override final;
        unit getDeltaTime() noexcept override final;
    
      private:
        Logging::Bind logger={"Timer", "STD"};
        high_resolution_clock clock;
        high_resolution_clock::time_point lastTimePoint = clock.now();
    };
}