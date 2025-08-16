#ifndef MODERNBOY_METAL_MATERIAL_HPP
#define MODERNBOY_METAL_MATERIAL_HPP

#include "engine/fwd.hpp"

namespace ModernBoy::Metal
{
    struct PBRMaterial{
        NativePtr material;

        PBRMaterial(NativePtr rctxPtr, NativePtr shaderPtr,
            // Textures
            NativePtr baseColorPtr, NativePtr normalPtr,
            NativePtr mrPtr, NativePtr emissivePtr);
        ~PBRMaterial();

        PBRMaterial()=default;
        PBRMaterial(const PBRMaterial&)=delete;
        PBRMaterial(PBRMaterial&&);
        PBRMaterial& operator=(const PBRMaterial&)=delete;
        PBRMaterial& operator=(PBRMaterial&&);

    private:
        // Move semantics
        void moveFrom(PBRMaterial&&);
    };
}

#endif // MODERNBOY_METAL_MATERIAL_HPP