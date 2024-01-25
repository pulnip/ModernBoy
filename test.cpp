#include <iostream>
#include <limits>
#include <cmath>

int main(void){
    double inf1 = std::numeric_limits<double>::infinity();

    double inf2=inf1*inf1;
    double v=42.0;

    std::cout<<std::min(0.02, inf1)<<std::endl;
    std::cout<<std::isnan(inf1 * 0.0)<<std::endl;

    return 0;
}