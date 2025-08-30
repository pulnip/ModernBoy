#ifndef MODERNBOY_ASSET_KTX2_ENCODER_H
#define MODERNBOY_ASSET_KTX2_ENCODER_H

#include <cstdint>
#include <ktx.h>

namespace ModernBoy::Asset{
    enum TextureKind{
        TEXTURE_BASECOLOR,
        TEXTURE_EMISSIVE,
        TEXTURE_NORMAL,
        TEXTURE_ORM
    };

    constexpr auto GENERATE_MIPMAPS = (1 << 0);
    constexpr auto USE_UASTC        = (1 << 1);

    struct Ktx2CookParams{
        TextureKind kind;
        uint32_t flag;
        uint8_t uastcLevel;
        uint8_t etc1sQuality;
        uint8_t zstdLevel;
    };

    int cook_image_to_ktx2_file(
        const char* imagePath,
        const char* ktx2Path,
        const Ktx2CookParams* params
    );
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_KTX2_ENCODER_H