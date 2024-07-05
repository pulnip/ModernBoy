#pragma once

#include <string>
#include <glm/glm.hpp>

namespace ModernBoy{
    struct WindowInfo{
        std::wstring title;
        glm::ivec2 position;
        glm::ivec2 size;
    };
}
