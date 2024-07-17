#pragma once

#include <cassert>
#include <concepts>
#include <iterator>
#include <vector>

namespace ModernBoy{
    namespace Algorithm{
        template<typename T> concept Int=std::integral<T>;
        template<typename T> concept R_It=std::random_access_iterator<T>;

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

        class Sieve{
            inline static std::vector<bool> odds;

            static void mark(Int auto pn, Int auto p) noexcept{
                auto end = odds.size();
                assert(pn < end);

                for(auto i=half(pn-3); i<end; i+=p){
                    odds[i]=false;
                }
            }

            template<Int N> static void sift(N unchecked) noexcept{
                auto m_1 = pow2(odds.size(), 1) + 3;

                for(N p=3; p*p < m_1; p+=2){
                    auto pp = p*p;
                    auto pn = unchecked - unchecked%p;
                    auto first = std::max(pn, pp);
                
                    mark(first, p);
                }
            }

          public:
            static bool isPrime(Int auto n){
                assert(2<n && odd(n));

                auto unchecked=pow2(odds.size(), 1)+3;
                auto idx = half(n-3);

                if(unchecked <= n){
                    odds.resize(idx + 1, true);
                    sift(unchecked);
                }

                return odds[idx];
            }
        };

        bool isPrime(Int auto n) noexcept{
            switch(n){
            case 0:
                [[fallthrough]];
            case 1:
                return false;
            case 2:
                return true;
            default:
                if(odd(n))
                    return Sieve::isPrime(n);
                else
                    return false;
            }
        }
    }
}