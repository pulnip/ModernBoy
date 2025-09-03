#include <algorithm>
#include <iostream>
#include <print>
#include <span>
#include <vector>
#include <ktx.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "asset/ktx2_encoder.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Asset;

namespace{
    constexpr ktx_uint32_t VK_FORMAT_R8G8B8A8_UNORM = 37;
    constexpr ktx_uint32_t VK_FORMAT_R8G8B8A8_SRGB  = 43;

    auto ktx_fail(ktx_error_code_e e, const char* where){
        std::println(std::cerr, "[ktx2] {} failed: {}",
            where, int(e));
        return -1;
    }

    auto is_perceptual(TextureKind k){
        return k == TextureKind::TEXTURE_BASECOLOR ||
               k == TextureKind::TEXTURE_EMISSIVE;
    }

    // 최대 밉 레벨 수 계산 (1x1까지)
    auto calc_mip_count(size_t w, size_t h){
        size_t levels = 1;
        while(w > 1 || h > 1){
            w = std::max(1ul, w / 2);
            h = std::max(1ul, h / 2);
            ++levels;
        }
        return levels;
    }

    // 2x2 box filter downsampler
    auto downsample_rgba8_box(
        std::span<const uint8_t> src,
        size_t sw, size_t sh,
        std::vector<uint8_t>& dst,
        size_t dw, size_t dh
    ){
        const auto expW = std::max(1ul, sw/2);
        const auto expH = std::max(1ul, sh/2);
        if(dw!=expW || dh!=expH || sw<=0 || sh<=0)
            return false;
        if(src.size() < sw*sh*4)
            return false;

        dst.assign(dw*dh*4, 0u);
        const auto maxX = sw-1;
        const auto maxY = sh-1;

        auto pix_index = [sw](size_t x, size_t y){
            return (y * sw + x) * 4u;
        };

        for(size_t y = 0; y < dh; ++y){
            const auto y0 = std::clamp(2*y+0, 0ul, maxY);
            const auto y1 = std::clamp(2*y+1, 0ul, maxY);

            for(size_t x = 0; x < dw; ++x){
                const auto x0 = std::clamp(2*x+0, 0ul, maxX);
                const auto x1 = std::clamp(2*x+1, 0ul, maxX);

                const auto i00 = 4*(y0*sw + x0);
                const auto i10 = 4*(y0*sw + x1);
                const auto i01 = 4*(y1*sw + x0);
                const auto i11 = 4*(y1*sw + x1);

                const auto o = 4*(y*dw + x);
                for(size_t c = 0; c < 4; ++c){
                    const auto s = src[i00 + c] + src[i10 + c] +
                                   src[i01 + c] + src[i11 + c];
                    dst[o + c] = static_cast<uint8_t>((s + 2u) >> 2);
                }
            }
        }
        return true;
    }

    auto default_params(){
        Ktx2CookParams p{};
        p.kind = TextureKind::TEXTURE_BASECOLOR;
        p.flag = GENERATE_MIPMAPS | USE_UASTC;
        p.uastcLevel = 2;     // 0(fast) ~ 4(quality)
        p.etc1sQuality = 128; // ETC1S 1~255
        p.zstdLevel = 18;     // (kept for future; not used directly in bp)
        return p;
    }

    auto load_rgba8(const char* path,
        std::vector<uint8_t>& out, int& w, int& h
    ){
        int comp=0;
        unsigned char* pixels = stbi_load(path, &w, &h, &comp, 4);
        if(!pixels)
            return false;
        out.assign(4*w*h, 0);
        std::memcpy(out.data(), pixels, out.size());
        stbi_image_free(pixels);
        return true;
    }

    struct Ktx2RAII {
        ktxTexture2* ptr{nullptr};
        ~Ktx2RAII(){
            if(ptr)
                ktxTexture_Destroy(ktxTexture(ptr));
        }
    };

    auto make_ktx2_surface(
        size_t w, size_t h, bool perceptual, bool genMips,
        Ktx2RAII& out, ktxTextureCreateInfo& outCi
    ){
        std::memset(&outCi, 0, sizeof(outCi));
        outCi.vkFormat        = perceptual ?
            VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
        outCi.baseWidth       = w;
        outCi.baseHeight      = h;
        outCi.baseDepth       = 1;
        outCi.numDimensions   = 2;
        outCi.numLevels       = genMips ? calc_mip_count(w, h) : 1;
        outCi.numLayers       = 1;
        outCi.numFaces        = 1;
        outCi.isArray         = KTX_FALSE;
        outCi.generateMipmaps = KTX_FALSE;
        auto e = ktxTexture2_Create(&outCi, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &out.ptr);
        if(e != KTX_SUCCESS)
            return ktx_fail(e, "ktxTexture2_Create");

        if(perceptual)
            ktxTexture2_SetOETF(out.ptr, KHR_DF_TRANSFER_SRGB);
        else
            ktxTexture2_SetOETF(out.ptr, KHR_DF_TRANSFER_LINEAR);
        return 0;
    }

    auto upload_level0(ktxTexture2* tex,
        std::span<const uint8_t> rgba, size_t w, size_t h
    ){
        const auto imageSize = 4*w*h;
        if(rgba.size() < imageSize)
            return ktx_fail(KTX_INVALID_OPERATION, "upload_level0:size");

        auto e = ktxTexture_SetImageFromMemory(
            ktxTexture(tex), 0, 0, 0, rgba.data(), imageSize);
        if(e != KTX_SUCCESS)
            return ktx_fail(e, "ktxTexture_SetImageFromMemory");
        return 0;
    }

    auto build_mip_chain_cpu(
        ktxTexture2* tex,
        const std::vector<uint8_t>& level0, size_t w, size_t h,
        uint32_t numLevels
    ){
        if(numLevels <= 1)
            return 0;

        auto srcW = w, srcH = h;
        std::vector<uint8_t> prev = level0;
        for(uint32_t level = 1; level < numLevels; ++level){
            const auto dstW = std::max(1ul, srcW / 2);
            const auto dstH = std::max(1ul, srcH / 2);

            std::vector<uint8_t> next;
            if(!downsample_rgba8_box(prev, srcW, srcH, next, dstW, dstH))
                return ktx_fail(KTX_INVALID_OPERATION, "downsample_rgba8_box");

            const ktx_size_t dstSize = 4*dstW*dstH;
            auto e = ktxTexture_SetImageFromMemory(
                ktxTexture(tex), level, 0, 0, next.data(), dstSize);
            if(e != KTX_SUCCESS)
                return ktx_fail(e, "ktxTexture_SetImageFromMemory (mip)");

            prev.swap(next);
            srcW = dstW;
            srcH = dstH;
        }
        return 0;
    }

    auto fill_basis_params(const Ktx2CookParams& params){
        ktxBasisParams bp;
        std::memset(&bp, 0, sizeof(bp));
        bp.structSize  = sizeof(bp);
        bp.uastc       = (params.flag & USE_UASTC) != 0;
        bp.verbose     = false;
        bp.noSSE       = false;
        bp.threadCount = 0;
        bp.normalMap   = params.kind == TEXTURE_NORMAL;
        if(bp.uastc){
            static const ktx_pack_uastc_flag_bits_e LEVEL_FLAGS[5] = {
                KTX_PACK_UASTC_LEVEL_FASTEST,
                KTX_PACK_UASTC_LEVEL_FASTER,
                KTX_PACK_UASTC_LEVEL_DEFAULT,
                KTX_PACK_UASTC_LEVEL_SLOWER,
                KTX_PACK_UASTC_LEVEL_VERYSLOW
            };
            const int lv = std::clamp(int(params.uastcLevel), 0, 4);
            bp.uastcFlags = LEVEL_FLAGS[lv];
        }else {
            bp.compressionLevel = KTX_ETC1S_DEFAULT_COMPRESSION_LEVEL; // 0~5 (기본 2)
            bp.qualityLevel     = params.etc1sQuality;                 // 1~255
        }

        return bp;
    }

    auto write_ktx2(ktxTexture2* tex, const char* outPath){
        auto e = ktxTexture_WriteToNamedFile(ktxTexture(tex), outPath);
        if(e != KTX_SUCCESS)
            return ktx_fail(e, "ktxTexture_WriteToNamedFile");
        return 0;
    }
}

int Asset::cook_image_to_ktx2_file(
    const char* imagePath,
    const char* ktx2Path,
    const Ktx2CookParams* in_params
){
    if(!imagePath || !ktx2Path){
        std::println(std::cerr, "[ktx2] invalid path argument");
        return -1;
    }

    Ktx2CookParams params = in_params ? *in_params : default_params();

    std::vector<uint8_t> rgba;
    int w=0, h=0;
    if(!load_rgba8(imagePath, rgba, w, h)){
        std::println(std::cerr, "[ktx2] stbi_load failed: {}", imagePath);
        return -1;
    }

    ktxTextureCreateInfo ci{};
    Ktx2RAII tex;
    auto genMips = (params.flag & GENERATE_MIPMAPS) != 0;
    if(make_ktx2_surface(w, h,
        is_perceptual(params.kind), genMips, tex, ci)
    )
        return -1;

    if(upload_level0(tex.ptr, rgba, w, h))
        return -1;

    if(genMips && ci.numLevels > 1u)
        if(build_mip_chain_cpu(tex.ptr,
            rgba, w, h, ci.numLevels)
        )
            return -1;

    auto bp = fill_basis_params(params);
    if(ktxTexture2_CompressBasisEx(tex.ptr, &bp) != KTX_SUCCESS)
        return ktx_fail(KTX_INVALID_OPERATION, "ktxTexture2_CompressBasisEx");

    return write_ktx2(tex.ptr, ktx2Path);
}