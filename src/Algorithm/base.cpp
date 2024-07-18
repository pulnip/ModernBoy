#include <vector>
#include "Algorithm/base.hpp"

using namespace std;
using namespace gsl;
using namespace ModernBoy;
using namespace ModernBoy::Algorithm;

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
        auto m_1 = pow2(odds.size(), 1) + 3;

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

        auto unchecked=pow2(odds.size(), 1)+3;
        auto idx = half(n-3);

        if(unchecked <= n){
            odds.resize(idx + 1, true);
            sift(unchecked);
        }

        return odds[idx];
    }
};

bool Algorithm::isPrime(index n) noexcept{
    static Sieve sieve;

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
