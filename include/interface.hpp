#ifndef MODERNBOY_INTERFACE_HPP
#define MODERNBOY_INTERFACE_HPP

#include <concepts>
#include <cstddef>
#include "fwd.hpp"
#include "util/generator.hpp"

namespace ModernBoy
{
    template<typename T>
    concept Schedulable = requires (T t, DeltaTime dt){
        { t.yield_count() } -> std::convertible_to<size_t>;
        { t.updateTask(dt) } -> std::convertible_to<Generator<void>>;
        { t.updateTask(dt) } -> std::convertible_to<Generator<void>>;
    };
}

#endif // MODERNBOY_INTERFACE_HPP