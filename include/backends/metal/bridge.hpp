#ifndef MODERNBOY_METAL_BRIDGE_HPP
#define MODERNBOY_METAL_BRIDGE_HPP

namespace ModernBoy::Metal
{
#ifdef __cplusplus
extern "C"{
#endif

    extern void setupMetal(const void* layerPtr);
    extern void uploadVertices(const float* vertices, int count);

    extern void renderStart();
    extern void draw();
    extern void renderEnd();

#ifdef __cplusplus
}
#endif
}

#endif // MODERNBOY_METAL_BRIDGE_HPP