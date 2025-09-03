extern "C" {
    #include <ktx.h>
}
#include <vector>
#include <cstring>
#include <cstdlib>
#include "asset/ktx2_transcode.h"

namespace{
    ktx_transcode_fmt_e pick_fmt_bcn(int preferBC7){
        return preferBC7 ? KTX_TTF_BC7_RGBA : KTX_TTF_BC3_RGBA;
    }

}

extern "C" int ktx2_transcode_for_metal(
    const char* ktx2Path,
    int astcBlock, uint32_t srgb,
    GpuPayload* out
){
    if(!ktx2Path || !out)
        return -1;
    ktxTexture2* kt = nullptr;
    KTX_error_code rc = ktxTexture2_CreateFromNamedFile(
        ktx2Path, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kt);
    if(rc != KTX_SUCCESS || !kt)
        return -1;

    if(ktxTexture2_NeedsTranscoding(kt)){
        ktx_transcode_fmt_e fmt = KTX_TTF_ASTC_4x4_RGBA;
        rc = ktxTexture2_TranscodeBasis(kt, fmt, KTX_TF_HIGH_QUALITY);
        if(rc != KTX_SUCCESS){
            ktxTexture_Destroy((ktxTexture*)kt);
            return -1;
        }
    }

    const uint32_t levels = kt->numLevels;
    const uint32_t w0 = kt->baseWidth;
    const uint32_t h0 = kt->baseHeight;

    ktx_uint8_t* base = ktxTexture_GetData((ktxTexture*)kt);
    if(!base){
        ktxTexture_Destroy((ktxTexture*)kt);
        return -1;
    }

    std::vector<uint64_t> offs(levels), sizes(levels);
    uint64_t total = 0;
    for(uint32_t l=0; l<levels; ++l){
        ktx_size_t off=0;
        if(ktxTexture_GetImageOffset(
            (ktxTexture*)kt, l, 0, 0, &off
        ) != KTX_SUCCESS){
            ktxTexture_Destroy((ktxTexture*)kt);
            return -1;
        }
        ktx_size_t sz = ktxTexture_GetImageSize((ktxTexture*)kt, l);
        offs[l]  = (uint64_t)off;
        sizes[l] = (uint64_t)sz;
        total += (uint64_t)sz;
    }

    uint8_t* data = (uint8_t*)std::malloc(total);
    if(!data){
        ktxTexture_Destroy((ktxTexture*)kt);
        return -1;
    }
    std::vector<uint64_t> newOffs(levels);
    uint64_t cur = 0;
    for(uint32_t l=0; l<levels; ++l){
        std::memcpy(data+cur, base+offs[l], sizes[l]);
        newOffs[l] = cur; cur += sizes[l];
    }

    auto* offBuf = (uint64_t*)std::malloc(levels*sizeof(uint64_t));
    auto* sizBuf = (uint64_t*)std::malloc(levels*sizeof(uint64_t));
    if(!offBuf || !sizBuf){
        if(offBuf)
            std::free(offBuf);
        if(sizBuf)
            std::free(sizBuf);
        std::free(data);
        ktxTexture_Destroy((ktxTexture*)kt);
        return -1;
    }
    std::memcpy(offBuf, newOffs.data(), levels*sizeof(uint64_t));
    std::memcpy(sizBuf, sizes.data(),   levels*sizeof(uint64_t));

    std::memset(out, 0, sizeof(GpuPayload));
    out->width  = w0; out->height = h0; out->levels = levels;
    out->srgb   = (srgb?1u:0u);
    out->data   = data;
    out->levelOffsets = offBuf;
    out->levelSizes   = sizBuf;
    out->ownedData   = data;
    out->ownedOffsets = offBuf;
    out->ownedSizes   = sizBuf;

    out->kind  = PAYLOAD_ASTC;
    out->block = (
        astcBlock==4 || astcBlock==5 ||
        astcBlock==6 || astcBlock==8
    ) ? (uint32_t)astcBlock : 6u;

    ktxTexture_Destroy((ktxTexture*)kt);
    return 0;
}

extern "C" int ktx2_transcode_for_directx(
    const char* ktx2Path,
    int preferBC7, uint32_t srgb,
    GpuPayload* out
){
    if(!ktx2Path || !out)
        return -1;
    ktxTexture2* kt = nullptr;
    KTX_error_code rc = ktxTexture2_CreateFromNamedFile(
        ktx2Path, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kt);
    if(rc != KTX_SUCCESS || !kt)
        return -1;

    if(ktxTexture2_NeedsTranscoding(kt)){
        ktx_transcode_fmt_e fmt = pick_fmt_bcn(preferBC7);
        rc = ktxTexture2_TranscodeBasis(kt, fmt, KTX_TF_HIGH_QUALITY);
        if(rc != KTX_SUCCESS){
            ktxTexture_Destroy((ktxTexture*)kt);
            return -1;
        }
    }

    const uint32_t levels = kt->numLevels;
    const uint32_t w0 = kt->baseWidth;
    const uint32_t h0 = kt->baseHeight;

    ktx_uint8_t* base = ktxTexture_GetData((ktxTexture*)kt);
    if(!base){
        ktxTexture_Destroy((ktxTexture*)kt);
        return -1;
    }

    std::vector<uint64_t> offs(levels), sizes(levels);
    uint64_t total = 0;
    for(uint32_t l=0; l<levels; ++l){
        ktx_size_t off=0;
        if(ktxTexture_GetImageOffset(
            (ktxTexture*)kt, l, 0, 0, &off
        ) != KTX_SUCCESS){
            ktxTexture_Destroy((ktxTexture*)kt);
            return -1;
        }
        ktx_size_t sz = ktxTexture_GetImageSize((ktxTexture*)kt, l);
        offs[l]  = (uint64_t)off;
        sizes[l] = (uint64_t)sz;
        total += (uint64_t)sz;
    }

    uint8_t* data = (uint8_t*)std::malloc(total);
    if(!data){
        ktxTexture_Destroy((ktxTexture*)kt);
        return -1;
    }
    std::vector<uint64_t> newOffs(levels);
    uint64_t cur = 0;
    for(uint32_t l=0; l<levels; ++l){
        std::memcpy(data+cur, base+offs[l], sizes[l]);
        newOffs[l] = cur; cur += sizes[l];
    }

    auto* offBuf = (uint64_t*)std::malloc(levels*sizeof(uint64_t));
    auto* sizBuf = (uint64_t*)std::malloc(levels*sizeof(uint64_t));
    if(!offBuf || !sizBuf){
        if(offBuf)
            std::free(offBuf);
        if(sizBuf)
            std::free(sizBuf);
        std::free(data);
        ktxTexture_Destroy((ktxTexture*)kt);
        return -1;
    }
    std::memcpy(offBuf, newOffs.data(), levels*sizeof(uint64_t));
    std::memcpy(sizBuf, sizes.data(),   levels*sizeof(uint64_t));

    std::memset(out, 0, sizeof(GpuPayload));
    out->width  = w0; out->height = h0; out->levels = levels;
    out->srgb   = (srgb?1u:0u);
    out->data   = data;
    out->levelOffsets = offBuf;
    out->levelSizes   = sizBuf;
    out->ownedData   = data;
    out->ownedOffsets = offBuf;
    out->ownedSizes   = sizBuf;

    out->block = 4;
    out->kind  = preferBC7 ? PAYLOAD_BC7 : PAYLOAD_BC3;

    ktxTexture_Destroy((ktxTexture*)kt);
    return 0;
}

extern "C" void ktx2_free_payload(GpuPayload* p){
    if(!p)
        return;
    std::free(p->ownedData);
    std::free(p->ownedOffsets);
    std::free(p->ownedSizes);
    std::memset(p, 0, sizeof(GpuPayload));
}
