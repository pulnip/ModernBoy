#pragma once

#include <concepts>
#include <iterator>

namespace std{
    template<typename S>
    concept scalar=std::is_scalar_v<S>;
    template<typename P>
    concept pointer=std::is_pointer_v<P>;
}

namespace ModernBoy{
    template<typename T>
    concept multiplicative_monoid=requires(T lhs, T rhs){
        requires std::same_as<decltype(lhs*rhs), T>;
        requires std::same_as<std::remove_reference_t<decltype(lhs*=rhs)>, T>;
    };
    template<typename T>
    concept multiplicative=multiplicative_monoid<T> && requires(T lhs, T rhs){
        requires std::same_as<decltype(lhs/rhs), T>;
        requires std::same_as<std::remove_reference_t<decltype(lhs/=rhs)>, T>;
    };
    template<typename L, typename R>
    concept multiply_commutative=requires(L lhs, R rhs){
        requires std::same_as<decltype(lhs*rhs), decltype(rhs*lhs)>;
    };
    template<typename T>
    concept additive_monoid=requires(T lhs, T rhs){
        requires std::same_as<decltype(lhs+rhs), T>;
        requires std::same_as<std::remove_reference_t<decltype(lhs+=rhs)>, T>;
    };
    template<typename T>
    concept additive=additive_monoid<T> && requires(T lhs, T rhs){
        requires std::same_as<decltype(lhs-rhs), T>;
        requires std::same_as<std::remove_reference_t<decltype(lhs-=rhs)>, T>;
    };
    template<typename L, typename R>
    concept add_commutative=requires(L lhs, R rhs){
        requires std::same_as<decltype(lhs+rhs), decltype(rhs+lhs)>;
    };
    template<typename T>
    concept scalar_multiplicative=std::scalar<T> || 
    requires(T t){
        requires std::scalar<decltype(t.x)>;
        requires multiply_commutative<T, decltype(t.x)>;
        requires std::same_as<decltype(t.x*t), T>;
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

    template<typename T>
    concept Dim4=requires(T t){ t.x; t.y; t.z; t.w; };
    template<typename T>
    concept Dim3 = !Dim4<T> && requires(T t){ t.x; t.y; t.z; };
    template<typename T>
    concept Dim2 = !Dim4<T> && !Dim3<T> && requires(T t){ t.x; t.y; };
}
