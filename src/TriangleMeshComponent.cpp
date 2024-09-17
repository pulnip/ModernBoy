#include "TriangleMeshComponent.hpp"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace DirectX::Colors;
using namespace ModernBoy;

TriangleMeshComponent::TriangleMeshComponent(Actor& actor,
    const ComPtr<Device>& device,
    const string& texName)
: MeshComponent(actor, texName, device){
    constexpr float sqrt3=1.7321f;
    constexpr float triangleScale=2.0f;

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

    mesh.verticies=std::move(triangleVertices);
    mesh.polygons=std::move(trianglePolygons);
}

TriangleMeshComponent::~TriangleMeshComponent()=default;
