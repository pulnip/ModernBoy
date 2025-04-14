#ifndef MODERNBOY_METAL_BRIDGE_HPP
#define MODERNBOY_METAL_BRIDGE_HPP

namespace ModernBoy::Metal
{
#ifdef __cplusplus
extern "C"{
#endif

    extern void renderStart();
    extern void draw();
    extern void renderEnd();

#ifdef __cplusplus
}
#endif
}

#endif // MODERNBOY_METAL_BRIDGE_HPP