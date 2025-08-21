#ifndef MODERNBOY_METAL_MATERIAL_HPP
#define MODERNBOY_METAL_MATERIAL_HPP

#include <span>
#include "engine/fwd.hpp"

namespace ModernBoy::Metal
{
    struct UnlitMaterial{
        NativePtr material;

        UnlitMaterial(NativePtr rctxPtr,
            const std::string& filePath);
        UnlitMaterial(NativePtr rctxPtr,
            std::span<uint8_t> pixels,
            int width, int height);
        ~UnlitMaterial();

        UnlitMaterial()=default;
        UnlitMaterial(const UnlitMaterial&)=delete;
        UnlitMaterial(UnlitMaterial&&);
        UnlitMaterial& operator=(const UnlitMaterial&)=delete;
        UnlitMaterial& operator=(UnlitMaterial&&);

    private:
        // Move semantics
        void moveFrom(UnlitMaterial&&);
    };
}

#endif // MODERNBOY_METAL_MATERIAL_HPP