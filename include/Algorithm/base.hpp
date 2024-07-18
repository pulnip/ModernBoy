#pragma once

#include <cassert>
#include <gsl-lite/gsl-lite.hpp>
#include "Concepts.hpp"

namespace ModernBoy{

    constexpr bool odd(std::integral auto n) noexcept{ return n & 0b1; }
    constexpr bool even(std::integral auto n) noexcept{ return !odd(n); }
    template<std::integral N>
    constexpr N half(N n) noexcept{ return n >> 1; }
    template<std::integral N>
    constexpr N twice(N n) noexcept{ return n << 1; }

    template<std::integral N>
    constexpr N multiply_aux(N r, N n, N a) noexcept{
        while(true){
            if(odd(n)){
                r+=a;
                if(n==1)
                    return r;
            }

            a+=a;
            n=half(n);
        }
    }

    template<std::integral N>
    constexpr N multiply(N n, N a) noexcept{
        while(even(n)){
            a+=a;
            n=half(n);
        }

        if(n==1)
            return a;

        return multiply_aux(a, half(n-1), a+a);
    }

    bool isPrime(gsl::index n) noexcept;

    template<std::integral N> N gcd(N a, N b) noexcept{
        while(b != 0){
            a %= b;
            std::swap(a, b);
        }

        return a;
    }

    template<multiplicative A, std::integral N>
    constexpr auto pow(A c, A a, N n) noexcept{
        assert(n >= 0);
        while(true){
            if(odd(n)){
                c *= a;
                if(n==1)
                    return c;
            }
            n=half(n);
            a *= a;
        }
    }

    template<multiplicative A, std::integral N>
    constexpr auto pow(A a, N n){
        assert(n > 0);
        while(even(n)){
            a *= a;
            n = half(n);
        }
        if(n==1)
            return a;
        
        return pow(a, a*a, half(n-1));
    }
}
