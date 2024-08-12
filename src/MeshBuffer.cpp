#include "MeshBuffer.hpp"
#include "Ray.hpp"
#include "Shapes.hpp"

using namespace std;
using namespace DirectX::SimpleMath;
using namespace DirectX::Colors;
using namespace ModernBoy;

MeshBuffer::MeshBuffer(){
    constexpr float cubeScale=1.0f;
    auto cube=make_shared<MeshObject>();

    Vertex leftBottomBack{
        .position=cubeScale*Vector3(-1.0f, -1.0f, -1.0f),
        .normal{-1.0f, -1.0f, -1.0f},
        // .color=Black.v,
        .uv={0.0f, 1.0f}
    };
    Vertex leftBottomFront{
        .position=cubeScale*Vector3(-1.0f, -1.0f, 1.0f),
        .normal{-1.0f, -1.0f, 1.0f},
        // .color=Blue.v,
        .uv={0.0f, 1.0f}
    };
    Vertex leftTopBack{
        .position=cubeScale*Vector3(-1.0f, 1.0f, -1.0f),
        .normal{-1.0f, 1.0f, -1.0f},
        // .color=Green.v,
        .uv={0.0f, 0.0f}
    };
    Vertex leftTopFront{
        .position=cubeScale*Vector3(-1.0f, 1.0f, 1.0f),
        .normal{-1.0f, 1.0f, 1.0f},
        // .color=Cyan.v,
        .uv={0.0f, 0.0f}
    };
    Vertex rightBottomBack{
        .position=cubeScale*Vector3(1.0f, -1.0f, -1.0f),
        .normal{1.0f, -1.0f, -1.0f},
        // .color=Red.v,
        .uv={1.0f, 1.0f}
    };
    Vertex rightBottomFront{
        .position=cubeScale*Vector3(1.0f, -1.0f, 1.0f),
        .normal{1.0f, -1.0f, 1.0f},
        // .color=Magenta.v,
        .uv={1.0f, 1.0f}
    };
    Vertex rightTopBack{
        .position=cubeScale*Vector3(1.0f, 1.0f, -1.0f),
        .normal{1.0f, 1.0f, -1.0f},
        // .color=Yellow.v,
        .uv={1.0f, 0.0f}
    };
    Vertex rightTopFront{
        .position=cubeScale*Vector3(1.0f, 1.0f, 1.0f),
        .normal{1.0f, 1.0f, 1.0f},
        // .color=White.v,
        .uv={1.0f, 0.0f}
    };

    std::vector cubeVertices{
        leftTopBack,
        leftTopFront,
        rightTopFront,
        rightTopBack,
        leftBottomBack,
        leftBottomFront,
        rightBottomFront,
        rightBottomBack
    };
    std::vector<Polygon> cubePolygons{
        {0, 1, 2}, {0, 2, 3},
        {0, 4, 5}, {0, 5, 1},
        {0, 3, 7}, {0, 7, 4},
        {3, 2, 7}, {2, 6, 7},
        {1, 5, 6}, {1, 6, 2},
        {4, 6, 5}, {4, 7, 6}
    };

    cube->mesh.verticies=std::move(cubeVertices);
    cube->mesh.polygons=std::move(cubePolygons);

    objects.emplace_back(std::move(cube));

    constexpr float sqrt3=1.7321f;
    constexpr float triangleScale=2.0f;
    auto triangle=std::make_shared<MeshObject>();

    Vertex leftBottom{
        .position=triangleScale*Vector3{-sqrt3/2, -0.5f, 0.0f},
        .normal{0.0f, 0.0f, -1.0f},
        // .color=Red.v,
        .uv={0.0f, 1.0f}
    };
    Vertex midTop{
        .position=triangleScale*Vector3{0.0f, 1.0f, 0.0f},
        .normal{0.0f, 0.0f, -1.0f},
        // .color=Green.v,
        .uv={0.5f, 0.0f}
    };
    Vertex rightBottom{
        .position=triangleScale*Vector3{sqrt3/2, -0.5f, 0.0f},
        .normal{0.0f, 0.0f, -1.0f},
        // .color=Blue.v,
        .uv={1.0f, 1.0f}
    };

    std::vector triangleVertices{
        leftBottom,
        midTop,
        rightBottom
    };
    std::vector<Polygon> trianglePolygons{
        {0, 1, 2}
    };

    triangle->mesh.verticies=std::move(triangleVertices);
    triangle->mesh.polygons=std::move(trianglePolygons);

    objects.emplace_back(std::move(triangle));
}

tuple<vector<Vertex>, vector<uint16_t>> MeshBuffer::extract(){
    vector<Vertex> vertices;
    vector<uint16_t> indices;

    size_t indexOffset=0;
    for(const auto& o: objects){
        vertices.reserve(vertices.size()+o->mesh.verticies.size());
        for(const auto& v: o->mesh.verticies){
            vertices.emplace_back(v);
        }

        indices.reserve(indices.size() + 3*o->mesh.polygons.size());
        for(const auto& p: o->mesh.polygons){
            indices.emplace_back(indexOffset + p.index[0]);
            indices.emplace_back(indexOffset + p.index[1]);
            indices.emplace_back(indexOffset + p.index[2]);
        }

        indexOffset += o->mesh.verticies.size();
    }

    return {vertices, indices};
}
