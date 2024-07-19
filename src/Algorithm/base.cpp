#include <vector>
#include "Algorithm/base.hpp"

using namespace std;
using namespace gsl;
using namespace ModernBoy;

namespace ModernBoy{
    class Sieve{
        vector<bool> odds;

        void mark(index pn, index p) noexcept{
            auto end = odds.size();
            assert(pn < end);

            for(auto i=half(pn-3); i<end; i+=p){
                odds[i]=false;
            }
        }

        void sift(index unchecked) noexcept{
            auto m_1 = twice(odds.size()) + 3;

            for(index p=3; p*p < m_1; p+=2){
                auto pp = p*p;
                auto pn = unchecked - unchecked%p;
                auto first = std::max(pn, pp);
            
                mark(first, p);
            }
        }

      public:
        bool isPrime(index n){
            assert(2<n && odd(n));

            auto unchecked = twice(odds.size()) + 3;
            auto idx = half(n-3);

            if(unchecked <= n){
                odds.resize(idx + 1, true);
                sift(unchecked);
            }

            return odds[idx];
        }
    };
}

bool ModernBoy::isPrime(index n) noexcept{
    constinit static Sieve sieve;

    switch(n){
    case 0:
        [[fallthrough]];
    case 1:
        return false;
    case 2:
        return true;
    default:
        if(odd(n))
            return sieve.isPrime(n);
        else
            return false;
    }
}
