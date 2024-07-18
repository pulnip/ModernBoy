#pragma once

#include <cassert>
#include <gsl-lite/gsl-lite.hpp>
#include "Concepts.hpp"

namespace ModernBoy{

    bool odd(Int auto n) noexcept{ return n & 0b1; }
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
}
