#ifndef __INC_METAL_BRIDGE_HPP
#define __INC_METAL_BRIDGE_HPP

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

#endif // __INC_METAL_BRIDGE_HPP