#include <gsl/gsl-lite.hpp>
#include "Math.hpp"

using namespace std;
using namespace gsl;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

RGBA ModernBoy::toRGBA(const RGB& color, Channel alpha) noexcept{
    return {color.r, color.g, color.b, alpha};
}
RGBA ModernBoy::rgbcvt(const Color& color) noexcept{
    return {
        narrow_cast<Channel>(255*color.R()),
        narrow_cast<Channel>(255*color.G()),
        narrow_cast<Channel>(255*color.B()),
        narrow_cast<Channel>(255*color.A()),
    };
}

Color ModernBoy::rgbcvt(const RGBA& color) noexcept{
    return {color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f};
}
