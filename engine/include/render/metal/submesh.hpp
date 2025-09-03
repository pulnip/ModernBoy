#ifndef MODERNBOY_METAL_MESH_HPP
#define MODERNBOY_METAL_MESH_HPP

#include <cstdint>
#include <span>
#include "asset/asset_format.hpp"
#include "fwd.hpp"

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
            std::span<const Asset::Vertex>,
            std::span<const uint32_t>);
        ~Submesh();

    private:
        // Move semantics
        void moveFrom(Submesh&&);
    };
}

#endif // MODERNBOY_METAL_MESH_HPP