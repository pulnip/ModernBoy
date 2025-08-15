#ifndef MODERNBOY_GENERATOR_HPP
#define MODERNBOY_GENERATOR_HPP

#include <coroutine>
#include <exception>
#include <optional>

namespace ModernBoy
{
    template<typename T>
    struct Generator{
        struct promise_type{
            std::optional<T> value;
            std::suspend_always yield_value(T t) noexcept{
                value = std::move(t);
                return {};
            }
            std::suspend_always initial_suspend() noexcept{
                return {};
            }
            std::suspend_always final_suspend() noexcept{
                return {};
            }
            Generator get_return_object(){ 
                return Generator{   
                    std::coroutine_handle<promise_type>
                        ::from_promise(*this)
                };
            }
            void return_void(){}
            void unhandled_exception(){ std::terminate(); }
        };

        using handle_type = std::coroutine_handle<promise_type>;
        handle_type coro;

        Generator(handle_type h):coro(h){}
        ~Generator(){
            if(coro)
                coro.destroy();
        }
        Generator(const Generator&) = delete;
        Generator(Generator&& other) noexcept
        :coro(other.coro){ other.coro = nullptr; }
        Generator& operator=(const Generator&) = delete;
        Generator& operator=(Generator&& other) noexcept{
            if(this != &other){
                if(coro)
                    coro.destroy();
                coro = other.coro;
                other.coro = nullptr;
            }
            return *this;
        }
        bool next() {
            if(!coro.done())
                coro.resume();
            return !coro.done();
        }
        T value() const{
            return coro.promise().current_value.value();
        }
        bool done() const{
            return coro.done();
        }
    };

    template<>
    struct Generator<void>{
        struct promise_type{
            std::suspend_always yield_value(int) noexcept{
                return {};
            }
            std::suspend_always initial_suspend() noexcept{
                return {};
            }
            std::suspend_always final_suspend() noexcept{
                return {};
            }
            Generator get_return_object(){ 
                return Generator{   
                    std::coroutine_handle<promise_type>
                        ::from_promise(*this)
                };
            }
            void return_void(){}
            void unhandled_exception(){ std::terminate(); }
        };

        using handle_type = std::coroutine_handle<promise_type>;
        handle_type coro;

        Generator(handle_type h):coro(h){}
        ~Generator(){
            if(coro)
                coro.destroy();
        }
        Generator(const Generator&) = delete;
        Generator(Generator&& other) noexcept
        :coro(other.coro){ other.coro = nullptr; }
        Generator& operator=(const Generator&) = delete;
        Generator& operator=(Generator&& other) noexcept{
            if(this != &other){
                if(coro)
                    coro.destroy();
                coro = other.coro;
                other.coro = nullptr;
            }
            return *this;
        }
        bool next() {
            if(!coro.done())
                coro.resume();
            return !coro.done();
        }
        bool done() const{
            return coro.done();
        }
    };
}

#endif // MODERNBOY_GENERATOR_HPP