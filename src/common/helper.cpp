#include <algorithm>
#include "common/helper.hpp"

using namespace ModernBoy;

std::string ModernBoy::toUpper(const std::string& text){
    std::string upper = text;
    std::transform(text.begin(), text.end(), upper.begin(),
        [](unsigned char c){ return std::toupper(c); }
    );
    return upper;
}