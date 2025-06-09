#ifndef MODERNBOY_METAL_MESH_HPP
#define MODERNBOY_METAL_MESH_HPP

#include <cstdint>
#include "fwd.hpp"
#include "type.hpp"
#include "resource_data.hpp"

namespace ModernBoy::Metal
{
#ifdef __cplusplus
extern "C"{
#endif

    extern void* makeTriangle(const void* layerPtr);
    extern void destroyMesh(const void* meshPtr);

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
        
        Mesh(MeshPtr meshPtr):meshPtr(meshPtr){}
        ~Mesh(){ destroyMesh(meshPtr); }

    private:
        // Move semantics
        void moveFrom(Mesh&&);
    };
}

#endif // MODERNBOY_METAL_MESH_HPP