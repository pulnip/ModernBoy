#pragma once

#include <memory>
#include <tuple>
#include <vector>
#include "Math.hpp"
#include "Object.hpp"
#include "Vertex.hpp"

namespace ModernBoy{
    class MeshBuffer{
        using VertexBuffer=std::vector<Vertex>;
        using IndexBuffer=std::vector<uint16_t>;

        std::vector<std::shared_ptr<MeshObject>> objects;

      public:
        MeshBuffer();
        std::tuple<VertexBuffer, IndexBuffer> extract();
    };
}
