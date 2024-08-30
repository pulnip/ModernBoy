#include "CubeMeshComponent.hpp"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace DirectX::Colors;
using namespace ModernBoy;

CubeMeshComponent::CubeMeshComponent(Actor& actor,
    const ComPtr<ID3D11Device>& device,
    const string& texName)
: MeshComponent(actor, texName, device){
    constexpr float cubeScale=1.0f;

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

    mesh.verticies=std::move(cubeVertices);
    mesh.polygons=std::move(cubePolygons);
}

CubeMeshComponent::~CubeMeshComponent()=default;
