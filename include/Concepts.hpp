#pragma once

#include <concepts>
#include <iterator>

namespace std{
    template<typename S>
    concept scalar=std::is_scalar_v<S>;
}

namespace ModernBoy{
    template<typename T>
    concept multiplicative=requires(T lhs, T rhs){
        requires std::regular<T>;
        lhs*rhs;
        requires std::same_as<decltype(lhs*rhs), T>;
    };
    template<typename T>
    concept additive=requires(T lhs, T rhs){
        requires std::regular<T>;
        lhs+rhs;
        requires std::same_as<decltype(lhs+rhs), T>;
    };
    template<typename S, typename T>
    concept scalar_multiplicative=requires(S s, T t){
        requires std::regular<T> && std::scalar<S>;
        s*t; t*s;
        requires std::same_as<decltype(s*t), T> && std::same_as<decltype(t*s), T>;
    };
}
