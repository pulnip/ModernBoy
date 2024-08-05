#pragma once

#include <string>
#include "Math.hpp"

namespace ModernBoy{
    struct WindowDesc{
        std::string title;
        ipoint2 position;
        ipoint2 size;
        ipoint2 resolution;
    };
}
