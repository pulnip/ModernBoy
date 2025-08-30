#ifndef MODERNBOY_RENDER_KTX2_TRANSCODE_H
#define MODERNBOY_RENDER_KTX2_TRANSCODE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PayloadKind{
    PAYLOAD_ASTC = 0,
    PAYLOAD_BC1 = 1,
    PAYLOAD_BC3 = 2,
    PAYLOAD_BC5 = 3,
    PAYLOAD_BC7 = 4,
} PayloadKind;

typedef struct{
    uint32_t width, height, levels;
    PayloadKind kind;
    uint32_t block;
    uint32_t srgb;

    const uint64_t* levelOffsets;
    const uint64_t* levelSizes;
    const uint8_t* data;

    void* ownedOffsets;
    void* ownedSizes;
    void* ownedData;
} GpuPayload;

int ktx2_transcode_for_metal(
    const char* ktx2Path,
    int astcBlock, uint32_t srgb,
    GpuPayload* out
);
int ktx2_transcode_for_directx(
    const char* ktx2Path,
    int preferBC7, uint32_t srgb,
    GpuPayload* out
);

void ktx2_free_payload(GpuPayload*);

#ifdef __cplusplus
}
#endif

#endif // MODERNBOY_RENDER_KTX2_TRANSCODE_H