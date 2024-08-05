#include "MeshBuffer.hpp"
#include "Ray.hpp"
#include "Shapes.hpp"

using namespace std;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

MeshBuffer::MeshBuffer(){
    constexpr float scale=1.0f;
    auto cube=make_shared<MeshObject>();

    Vertex leftBottomBack{
        .position=scale*Vector3(-1.0f, -1.0f, -1.0f),
        .normal{-1.0f, -1.0f, -1.0f},
        .color{0.0f, 0.0f, 0.0f}
    };
    Vertex leftBottomFront{
        .position=scale*Vector3(-1.0f, -1.0f, 1.0f),
        .normal{-1.0f, -1.0f, 1.0f},
        .color{0.0f, 0.0f, 1.0f}
    };
    Vertex leftTopBack{
        .position=scale*Vector3(-1.0f, 1.0f, -1.0f),
        .normal{-1.0f, 1.0f, -1.0f},
        .color{0.0f, 1.0f, 0.0f}
    };
    Vertex leftTopFront{
        .position=scale*Vector3(-1.0f, 1.0f, 1.0f),
        .normal{-1.0f, 1.0f, 1.0f},
        .color{0.0f, 1.0f, 1.0f}
    };
    Vertex rightBottomBack{
        .position=scale*Vector3(1.0f, -1.0f, -1.0f),
        .normal{1.0f, -1.0f, -1.0f},
        .color{1.0f, 0.0f, 0.0f}
    };
    Vertex rightBottomFront{
        .position=scale*Vector3(1.0f, -1.0f, 1.0f),
        .normal{1.0f, -1.0f, 1.0f},
        .color{1.0f, 0.0f, 1.0f}
    };
    Vertex rightTopBack{
        .position=scale*Vector3(1.0f, 1.0f, -1.0f),
        .normal{1.0f, 1.0f, -1.0f},
        .color{1.0f, 1.0f, 0.0f}
    };
    Vertex rightTopFront{
        .position=scale*Vector3(1.0f, 1.0f, 1.0f),
        .normal{1.0f, 1.0f, 1.0f},
        .color{1.0f, 1.0f, 1.0f}
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

    // objects.emplace_back(std::move(cube));

    constexpr float sqrt3=1.7321f;
    auto triangle=std::make_shared<MeshObject>();

    Vertex leftBottom{
        .position=scale*Vector3{-sqrt3/2, -0.5f, 0.0f},
        .normal{0.0f, 0.0f, -1.0f},
        .color{1.0f, 0.0f, 0.0f}
    };
    Vertex midTop{
        .position=scale*Vector3{0.0f, 1.0f, 0.0f},
        .normal{0.0f, 0.0f, -1.0f},
        .color{0.0f, 1.0f, 0.0f}
    };
    Vertex rightBottom{
        .position=scale*Vector3{sqrt3/2, -0.5f, 0.0f},
        .normal{0.0f, 0.0f, -1.0f},
        .color{0.0f, 0.0f, 1.0f}
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

tuple<vector<ColorVertex>, vector<uint16_t>> MeshBuffer::extract(){
    vector<ColorVertex> vertices;
    vector<uint16_t> indices;

    for(const auto& o: objects){
        vertices.reserve(vertices.size()+o->mesh.verticies.size());
        for(const auto& v: o->mesh.verticies){
            vertices.emplace_back(ColorVertex{
                .position=v.position,
                .color=v.color
            });
        }

        indices.reserve(indices.size() + 3*o->mesh.polygons.size());
        for(const auto& p: o->mesh.polygons){
            indices.emplace_back(p.index[0]);
            indices.emplace_back(p.index[1]);
            indices.emplace_back(p.index[2]);
        }
    }

    return {vertices, indices};
}
