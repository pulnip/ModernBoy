#pragma once

#include <concepts>
#include <iterator>

namespace ModernBoy{
    template<typename T>
    concept multiplicative=requires(T lhs, T rhs){
        requires std::regular<T>;
        requires std::same_as<decltype(lhs*rhs), T>;
    };
}
