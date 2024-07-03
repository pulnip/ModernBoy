#pragma once

#include <string>
#include "Vector.hpp"

namespace ModernBoy{
    struct WindowInfo{
        std::wstring title;
        Vector2 position;
        Vector2 size;
    };
}
