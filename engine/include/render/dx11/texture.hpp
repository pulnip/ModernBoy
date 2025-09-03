#ifndef MODERNBOY_DX11_TEXTURE_HPP
#define MODERNBOY_DX11_TEXTURE_HPP

#include "fwd.hpp"

namespace ModernBoy::DX11
{
    struct Texture{
        NativePtr texture;

        Texture(NativePtr rctxPtr,
            const std::string& fileName);
        Texture(NativePtr rctxPtr,
            uint8_t* pixel, int width, int height);
        ~Texture();

        Texture()=default;
        Texture(const Texture&)=delete;
        Texture(Texture&&);
        Texture& operator=(const Texture&)=delete;
        Texture& operator=(Texture&&);

    private:
        // Move semantics
        void moveFrom(Texture&&);
    };
}

#endif // MODERNBOY_DX11_TEXTURE_HPP