#pragma once

#include <concepts>
#include <iterator>

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
    template<typename T>
    concept scalar=std::integral<T> && std::floating_point<T>;
    template<typename L, typename R>
    concept scalar_multiplicative=requires(L lhs, R rhs){
        requires std::regular<L> && std::regular<R>;
        lhs*rhs;
        (scalar<L> && std::same_as<decltype(lhs*rhs), R>) ||
        (scalar<R> && std::same_as<decltype(lhs*rhs), L>);
    };
}
