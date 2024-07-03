#pragma once

namespace ModernBoy{
    struct Vector2{
        union{ float x, w; };
        union{ float y, h; };
    };

    struct Vector3{
        union{ float x, r, w; };
        union{ float y, g, h; };
        union{ float z, b, d; };
    };
}
