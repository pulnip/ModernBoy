#ifndef MODERNBOY_METAL_MESH_HPP
#define MODERNBOY_METAL_MESH_HPP

#include <cstdint>
#include <span>
#include "engine/asset/asset_format.hpp"
#include "engine/fwd.hpp"

namespace ModernBoy::Metal
{
    struct Submesh{
        NativePtr nativeMesh;

        Submesh()=default;
        Submesh(const Submesh&)=delete;
        Submesh(Submesh&& mesh);
        Submesh& operator=(const Submesh&)=delete;
        Submesh& operator=(Submesh&&);

        Submesh(NativePtr rctxPtr,
            std::span<Asset::Vertex>,
            std::span<uint32_t>);
        ~Submesh();

    private:
        // Move semantics
        void moveFrom(Submesh&&);
    };
}

#endif // MODERNBOY_METAL_MESH_HPP