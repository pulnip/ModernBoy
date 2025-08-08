#ifndef MODERNBOY_INTERFACE_HPP
#define MODERNBOY_INTERFACE_HPP

#include <concepts>
#include <cstddef>
#include "fwd.hpp"
#include "core/thread/generator.hpp"

namespace ModernBoy
{
    template<typename T>
    concept Schedulable = requires (T t, DeltaTime dt){
        { t.yield_count() } -> std::convertible_to<size_t>;
        { t.updateTask(dt) } -> std::convertible_to<Generator<void>>;
    } || requires(T t, DeltaTime dt){
        { t.yield_count() } -> std::convertible_to<size_t>;
        { t.update(dt) } -> std::convertible_to<Generator<void>>;
    };

    struct TaskPolicy{
        size_t effective_window_size;
        TaskTime min_interval, patience;
    };

    template<typename T>
    concept Subsystem = requires(T t, DeltaTime dt){
        { t.expectedExecTime() } -> std::convertible_to<TaskTime>;
    };
}

#endif // MODERNBOY_INTERFACE_HPP