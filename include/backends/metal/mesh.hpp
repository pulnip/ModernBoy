#ifndef MODERNBOY_METAL_MESH_HPP
#define MODERNBOY_METAL_MESH_HPP

#include <cstdint>
#include "fwd.hpp"
#include "type.hpp"
#include "resource_data.hpp"

namespace ModernBoy::Metal
{
    struct Mesh{
        BufferPtr vertexBuffer;
        uint16_t numVertices;
        BufferPtr indexBuffer;
        uint16_t numIndices;

    public:
        Mesh()=default;
        ~Mesh()=default;
        Mesh(const Mesh&)=delete;
        Mesh(Mesh&& mesh);
        Mesh& operator=(const Mesh&)=delete;
        Mesh& operator=(Mesh&&);

        void bind(RenderContext& in_context) const;

    private:
        // Move semantics
        void moveFrom(Mesh&&);
    };
    // static_assert(ResourceData<Mesh, RenderContext>);
#ifdef __cplusplus
extern "C"{
#endif

    extern void* makeTriangle(const float* vertices, int count);

#ifdef __cplusplus
}
#endif
}

#endif // MODERNBOY_METAL_MESH_HPP