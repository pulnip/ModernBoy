#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include "Math.hpp"
#include "Texture.hpp"

using namespace std;
using namespace gsl;
using namespace glm;
using namespace ModernBoy;

Texture::Texture(const string& fileName){
    auto img = stbi_load(fileName.c_str(), &size.x, &size.y, &channels, 0);

	image.resize(size.x * size.y * channels);
	memcpy(image.data(), img, image.size() * sizeof(Channel));

	delete img;
}

Texture::Texture(const PixelPos& size, const vector<fRGB>& pixels)
:size(size), channels(3){
    const auto width=size.x;
    const auto height=size.y;

    image.resize(width*height*channels);

	for (index i=0; i<height; ++i){
        const auto floor=width*i;
        for (index j=0; j<width; ++j){
            const auto index=floor+j;
            const auto color=rgbcvt(pixels[index]);

            const auto chIndex=channels*index;
			image[chIndex + 0]=color.r;
			image[chIndex + 1]=color.g;
			image[chIndex + 2]=color.b;
		}
    }
}

fRGB Texture::getRGB(const PixelPos& pixelPos) noexcept{
    const auto wrapped = wrap(pixelPos, {{0, 0}, size});
    const auto chIndex = getIndex(wrapped);
    return getRGBf(chIndex);
}

fRGB Texture::getRGB(const UVPos& uv) noexcept{
    const auto nearestPos = toPixelPos(uv);
    return getRGB(nearestPos);
}

fRGB Texture::getRGB_lerp(const UVPos& uv) noexcept{
    const auto accuratePos = scaleUV(uv);
    const auto pos00 = narrow_cast<ivec2>(floor(accuratePos - vec2(0.5f)));
    const auto dx = accuratePos.x - pos00.x;
    const auto dy = accuratePos.y - pos00.y;

    const auto color00 = getRGB(pos00);
    const auto color10 = getRGB(pos00 + ivec2{1, 0});
    const auto color01 = getRGB(pos00 + ivec2{0, 1});
    const auto color11 = getRGB(pos00 + ivec2{1, 1});

    return lerp2(
        {color00, color10},
        {color01, color11},
        dx, dy
    );
}

vec2 Texture::scaleUV(const UVPos& uv) noexcept{
    return uv*static_cast<vec2>(size);
}

Texture::PixelPos Texture::toPixelPos(const UVPos& uv) noexcept{
    return wrap(
        narrow_cast<ivec2>(scaleUV(uv)),
        {{0, 0}, size}
    );
}

index Texture::getIndex(const PixelPos& pixelPos) noexcept{
    return channels * (size.x*pixelPos.x + pixelPos.y);
}

fRGB Texture::getRGBf(const index chIndex) noexcept{
    return rgbcvt(getRGBi(chIndex));
}

iRGB Texture::getRGBi(const index chIndex) noexcept{
    return ivec3{
        image[chIndex + 0],
        image[chIndex + 1],
        image[chIndex + 2]
    };
}
