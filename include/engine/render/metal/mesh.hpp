#ifndef MODERNBOY_METAL_MESH_HPP
#define MODERNBOY_METAL_MESH_HPP

#include <cstdint>
#include <span>
#include "engine/asset/asset_format.hpp"
#include "engine/fwd.hpp"

namespace ModernBoy::Metal
{
    using MeshPartPtr = NativePtr;

    struct Mesh{
        std::vector<MeshPartPtr> meshPtr;

        Mesh()=default;
        Mesh(const Mesh&)=delete;
        Mesh(Mesh&& mesh);
        Mesh& operator=(const Mesh&)=delete;
        Mesh& operator=(Mesh&&);

        Mesh(NativePtr rctxPtr, const std::string& fileName);
        Mesh(NativePtr rctxPtr, const Asset::CookedMesh&);
        Mesh(MeshPartPtr partPtr):meshPtr({partPtr}){}
        ~Mesh();

    private:
        // Move semantics
        void moveFrom(Mesh&&);
    };

    struct Submesh{
        NativePtr meshPtr;

        Submesh() = default;
        Submesh()=default;
        Submesh(const Submesh&)=delete;
        Submesh(Submesh&& mesh);
        Submesh& operator=(const Submesh&)=delete;
        Submesh& operator=(Submesh&&);

        Submesh(NativePtr rctxPtr,
            std::span<Asset::Vertex>,
            std::span<uint32_t>);
        ~Submesh();
    };
}

#endif // MODERNBOY_METAL_MESH_HPP