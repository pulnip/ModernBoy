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
        requires std::same_as<decltype(lhs*rhs), T>;
        requires std::same_as<std::remove_reference_t<decltype(lhs*=rhs)>, T>;
    };
    template<typename L, typename R>
    concept multiply_commutative=requires(L lhs, R rhs){
        requires std::same_as<decltype(lhs*rhs), decltype(rhs*lhs)>;
    };
    template<typename T>
    concept additive=requires(T lhs, T rhs){
        requires std::same_as<decltype(lhs+rhs), T>;
        requires std::same_as<std::remove_reference_t<decltype(lhs+=rhs)>, T>;
    };
    template<typename L, typename R>
    concept add_commutative=requires(L lhs, R rhs){
        requires std::same_as<decltype(lhs+rhs), decltype(rhs+lhs)>;
    };
    template<typename T>
    concept scalar_multiplicative=std::scalar<T> || 
    requires(typename T::value_type s, T t){
        requires std::scalar<decltype(s)>;
        requires multiply_commutative<T, decltype(s)>;
        requires std::same_as<decltype(s*t), T>;
    };

    template<typename T>
    concept linear=requires{
        requires std::regular<T>;
        requires additive<T> && scalar_multiplicative<T>;
    };
    template<typename P, typename V>
    concept point=requires(P p, V v){
        requires std::regular<P> && linear<V>;
        requires !scalar_multiplicative<P>;
        requires add_commutative<P, V>;
        requires std::same_as<decltype(p+v), P>;
        requires std::same_as<std::remove_reference_t<decltype(p+=v)>, P>;
    };
    template<typename V, typename P>
    concept affine_additive=linear<V> && point<P, V>;
}
