#ifndef MODERNBOY_THREAD_BACKOFF_HPP
#define MODERNBOY_THREAD_BACKOFF_HPP

#include <chrono>
#include <thread>

namespace ModernBoy
{
    template<int PassiveWaitLimit=16, int YieldLimit=64, int SleepMs=1>
    struct AdaptiveBackoff{
        int retries = 0;

        void reset(){ retries = 0; }

        void operator()(){
            if(retries < PassiveWaitLimit){
                // do Nothing (passive spin)
            } else if(retries < YieldLimit){
                std::this_thread::yield();
            } else{
                std::this_thread::sleep_for(std::chrono::milliseconds(SleepMs));
            }
            ++retries;
        }
    };

} // namespace ModernBoy

#endif // MODERNBOY_THREAD_BACKOFF_HPP