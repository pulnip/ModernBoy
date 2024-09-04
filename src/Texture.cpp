#include <print>
#include <SDL2/SDL_image.h>
#include "helper.hpp"
#include "Math.hpp"
#include "Texture.hpp"

using namespace std;
using namespace gsl;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

struct SDL_RAII_Surface{
    SDL_Surface* surface=nullptr;

    SDL_RAII_Surface(const string& fileName)
    : surface(IMG_Load(fileName.c_str())){
        throwIf(is_null(surface), IMG_GetError());
    }
    SDL_RAII_Surface(const string& fileName,
        const SDL_PixelFormatEnum format
    ){
        SDL_RAII_Surface raw(fileName);
        surface=SDL_ConvertSurfaceFormat(raw.surface, format, 0);
        throwIf(is_null(surface), IMG_GetError());
    }
    ~SDL_RAII_Surface(){
        SDL_FreeSurface(surface);
    }
};

Texture::Texture(const string& fileName, const ComPtr<ID3D11Device>& device){
    D3D11_TEXTURE2D_DESC td{};
    td.MipLevels=td.ArraySize=1;
    td.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count=1;
    td.Usage=D3D11_USAGE_IMMUTABLE;
    td.BindFlags=D3D11_BIND_SHADER_RESOURCE;

    // load texture from file
    SDL_RAII_Surface wrapped(fileName, SDL_PIXELFORMAT_RGBA32);
    SDL_Surface*& surf=wrapped.surface;

    // create texture
    println("Load {}({}x{}) {}", fileName, surf->w, surf->h, surf->pitch);
    td.Width=surf->w;
    td.Height=surf->h;
    D3D11_SUBRESOURCE_DATA textureData{
        .pSysMem=surf->pixels,
        .SysMemPitch=static_cast<UINT>(surf->pitch),
        .SysMemSlicePitch=static_cast<UINT>(surf->pitch*surf->h)
    };

    DX_throwIf(device->CreateTexture2D(&td, &textureData,
        texture.GetAddressOf()
    ));
    DX_throwIf(device->CreateShaderResourceView(texture.Get(), nullptr,
        texView.GetAddressOf()
    ));
}

SimpleTexture::SimpleTexture(const string& fileName){
    // load texture from file

    // int width, height, chnum;

    // uint8_t* img=stbi_load(name.c_str(),
    //     &width, &height,
    //     &chnum, 4
    // );
    // assert(chnum==4);

    auto surf=IMG_Load(fileName.c_str());

    size.x=surf->w;
    size.y=surf->h;
    channels=surf->format->BytesPerPixel;

	image.resize(size.x * size.y * channels);
	memcpy(image.data(), surf->pixels, image.size() * sizeof(Channel));
}

SimpleTexture::SimpleTexture(const PixelPos& size, const vector<Color>& pixels)
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

Color SimpleTexture::getRGB(const PixelPos& pixelPos) noexcept{
    const PixelPos wrapped = wrap<Vector2>(pixelPos, {{0, 0}, size});
    const auto chIndex = getIndex(wrapped);
    return getRGBf(chIndex);
}

Color SimpleTexture::getRGB(const UVPos& uv) noexcept{
    const auto nearestPos = toPixelPos(uv);
    return getRGB(nearestPos);
}

Color SimpleTexture::getRGB_lerp(const UVPos& uv) noexcept{
    const auto accuratePos = scaleUV(uv);
    const ipoint2 pos00{floor(accuratePos - Vector2(0.5f))};
    const auto dx = accuratePos.x - pos00.x;
    const auto dy = accuratePos.y - pos00.y;

    const auto color00 = getRGB(pos00);
    const auto color10 = getRGB(pos00 + Vector2{1, 0});
    const auto color01 = getRGB(pos00 + Vector2{0, 1});
    const auto color11 = getRGB(pos00 + Vector2{1, 1});

    return lerp2<Color>(
        {
            {color00, color10},
            {color01, color11},
        }, dx, dy
    );
}

Vector2 SimpleTexture::scaleUV(const UVPos& uv) noexcept{
    return {size.x*uv.x, size.y*uv.y};
}

PixelPos SimpleTexture::toPixelPos(const UVPos& uv) noexcept{
    return wrap<Vector2>(uv, {{0, 0}, size});
}

index SimpleTexture::getIndex(const PixelPos& pixelPos) noexcept{
    return channels * (size.x*pixelPos.y + pixelPos.x);
}

Color SimpleTexture::getRGBf(const index chIndex) noexcept{
    return rgbcvt(toRGBA(getRGBi(chIndex)));
}

RGB SimpleTexture::getRGBi(const index chIndex) noexcept{
    return {
        image[chIndex + 0],
        image[chIndex + 1],
        image[chIndex + 2]
    };
}
