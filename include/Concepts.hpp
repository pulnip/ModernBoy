#pragma once

#include <concepts>
#include <iterator>

namespace ModernBoy{
    template<typename T> concept Int=std::integral<T>;
    template<typename T> concept R_It=std::random_access_iterator<T>;

}