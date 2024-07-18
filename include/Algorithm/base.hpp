#pragma once

#include <cassert>
#include <gsl-lite/gsl-lite.hpp>
#include "Concepts.hpp"

namespace ModernBoy{

    bool odd(Int auto n) noexcept{ return n & 0b1; }
    bool even(Int auto n) noexcept{ return !odd(n); }
    Int auto half(Int auto n) noexcept{ return n >> 1; }
    Int auto pow2(Int auto x, Int auto n) noexcept{ return x << n; }

    Int auto multiply_aux(Int auto r, Int auto n, Int auto a) noexcept{
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

    Int auto multiply(Int auto n, Int auto a) noexcept{
        while(!odd(n)){
            a+=a;
            n=half(n);
        }

        if(n==1)
            return a;

        return multiply_aux(a, half(n-1), a+a);
    }

    bool isPrime(gsl::index n) noexcept;

    template<Int N> N gcd(N a, N b) noexcept{
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
