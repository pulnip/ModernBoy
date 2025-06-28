#ifndef MODERNBOY_METAL_MESH_HPP
#define MODERNBOY_METAL_MESH_HPP

#include <cstdint>
#include "fwd.hpp"

namespace ModernBoy::Metal
{
    using MeshPartPtr = NativePtr;

#ifdef __cplusplus
extern "C"{
#endif

    extern void* createMesh(const void* layerPtr,
        const float* vertices, int numVertices,
        const uint32_t* indices, int numIndices,
        const char* texturePath);
    extern void destroyMesh(const void* meshPtr);
    extern void* makeTriangle(const void* layerPtr);

#ifdef __cplusplus
}
#endif

    struct Mesh{
        std::vector<MeshPartPtr> meshPtr;

        Mesh()=default;
        Mesh(const Mesh&)=delete;
        Mesh(Mesh&& mesh);
        Mesh& operator=(const Mesh&)=delete;
        Mesh& operator=(Mesh&&);

        Mesh(RawMesh& rawMesh, AppState& app);
        Mesh(MeshPartPtr partPtr):meshPtr({partPtr}){}
        ~Mesh(){
            for(const auto& part: meshPtr)
                destroyMesh(part);
        }

    private:
        // Move semantics
        void moveFrom(Mesh&&);
    };
}

#endif // MODERNBOY_METAL_MESH_HPP