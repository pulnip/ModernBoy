#ifndef MODERNBOY_METAL_MESH_HPP
#define MODERNBOY_METAL_MESH_HPP

#include <cstdint>
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

        Mesh(const std::string& fileName, NativePtr metalLayer);
        Mesh(MeshPartPtr partPtr):meshPtr({partPtr}){}
        ~Mesh();

    private:
        // Move semantics
        void moveFrom(Mesh&&);
    };
}

#endif // MODERNBOY_METAL_MESH_HPP