#ifndef MODERNBOY_METAL_MESH_HPP
#define MODERNBOY_METAL_MESH_HPP

#include <cstdint>
#include "fwd.hpp"

namespace ModernBoy::Metal
{
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
    using MeshPtr = NativePtr;

    struct Mesh{
        MeshPtr meshPtr;

        Mesh()=default;
        Mesh(const Mesh&)=delete;
        Mesh(Mesh&& mesh);
        Mesh& operator=(const Mesh&)=delete;
        Mesh& operator=(Mesh&&);

        Mesh(RawMesh& rawMesh, AppState& app);
        Mesh(NativePtr meshPtr):meshPtr(meshPtr){}
        ~Mesh(){ destroyMesh(meshPtr); }

    private:
        // Move semantics
        void moveFrom(Mesh&&);
    };
}

#endif // MODERNBOY_METAL_MESH_HPP