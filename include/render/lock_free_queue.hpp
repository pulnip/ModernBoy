#ifndef __INC_RENDER_QUEUE_HPP
#define __INC_RENDER_QUEUE_HPP

#include <array>
#include <condition_variable>
#include <cstdint>
#include <iterator>
#include <concepts>
#include <mutex>
#include <queue>
#include <span>
#include <thread>
#include "render_command.hpp"
#include "resource_system.hpp"

namespace ModernBoy
{
    template<typename T, size_t Capacity=256>
    class LockFreeQueue{
    private:
        static constexpr size_t MASK = Capacity - 1;
        static_assert((Capacity & MASK)==0, "Capacity must be power of two");

        std::array<T, Capacity> buffer;
        std::atomic<size_t> head = 0; // write index
        std::atomic<size_t> tail = 0; // read index

    public:
        bool tryPush(const T& item){
            size_t h = head.load(std::memory_order_relaxed);
            size_t next = (h + 1) & MASK;

            if(next == tail.load(std::memory_order_acquire)){
                return false; // buffer full
            }

            buffer[h] = item;
            head.store(next, std::memory_order_release);
            return true;
        }
        bool tryPushAll(std::span<const T> items){
            for (const auto& item: items){
                if(!tryPush(item)) return false;
            }
            return true;
        }

        bool tryPop(T& out){
            size_t t = tail.load(std::memory_order_relaxed);
            if(t == head.load(std::memory_order_acquire)){
                return false; // buffer empty
            }

            out = buffer[t];
            tail.store((t + 1) & MASK, std::memory_order_release);
            return true;
        }
        template<std::output_iterator<T> OutputIt>
        bool tryPopAll(OutputIt dst) {
            bool poppedAny = false;
            T item;

            while(tryPop(item)){
                *dst++ = std::move(item);
                poppedAny = true;
            }

            return poppedAny;
        }

        bool empty() const{
            return head.load(std::memory_order_acquire) == tail.load(std::memory_order_acquire);
        }
        bool full() const{
            return ((head.load() + 1) & MASK) == tail.load();
        }

        size_t size() const{
            size_t h = head.load(std::memory_order_acquire);
            size_t t = tail.load(std::memory_order_acquire);
            return (h - t) & MASK;
        }
        consteval size_t capacity() const{
            return Capacity - 1; // 1 for distinct full/empty 
        }
    };

    template<int PassiveWaitLimit = 16, int YieldLimit = 64, int SleepMs=1>
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

    // unsafe!
    template<typename Queue, typename T, typename Backoff = AdaptiveBackoff<>>
    void waitUntilPushed(Queue& queue, const T& item,
        Backoff backoff = {})
    {
        while(!queue.tryPush(item)){ backoff(); }
    }
    template<typename Queue, typename T, typename Backoff = AdaptiveBackoff<>>
    void waitUntilPushed(Queue& queue, const T& item,
        std::stop_token stoken, Backoff backoff = {})
    {
        while(!queue.tryPush(item) && !stoken.stop_requested()){ backoff(); }
    }
    template<typename Queue, typename T, typename Rep, typename Period,
        typename Backoff = AdaptiveBackoff<>>
    bool waitUntilPushedFor(Queue& queue, const T& item,
        const std::chrono::duration<Rep, Period>& timeout, Backoff backoff = {})
    {
        auto start = std::chrono::steady_clock::now();
        while(!queue.tryPush(item)){
            if(std::chrono::steady_clock::now() - start >= timeout){
                return false;
            }
            backoff();
        }
        return true;
    }

    template<typename Queue, typename T, typename Backoff = AdaptiveBackoff<>>
    void waitUntilPopped(Queue& queue, T& out,
        Backoff backoff = {})
    {
        while(!queue.tryPop(out)){ backoff(); }
    }
    template<typename Queue, typename T, typename Backoff = AdaptiveBackoff<>>
    void waitUntilPopped(Queue& queue, T& out,
        std::stop_token stoken, Backoff backoff = {})
    {
        while(!queue.tryPop(out) && !stoken.stop_requested()){ backoff(); }
    }
    template<typename Queue, typename T, typename Rep, typename Period,
        typename Backoff = AdaptiveBackoff<>>
    bool waitUntilPoppedFor(Queue& queue, T& out,
        const std::chrono::duration<Rep, Period>& timeout, Backoff backoff = {})
    {
        auto start = std::chrono::steady_clock::now();
        while(!queue.tryPop(out)){
            if(std::chrono::steady_clock::now() - start >= timeout){
                return false;
            }
            backoff();
        }
        return true;
    }
} // namespace ModernBoy

#endif // __INC_RENDER_QUEUE_HPP