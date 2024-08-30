#include <directxmath/DirectXMath.h>
#include "CylinderMeshComponent.hpp"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace DirectX::Colors;
using namespace ModernBoy;

CylinderMeshComponent::CylinderMeshComponent(Actor& actor,
    const ComPtr<ID3D11Device>& device,
    const size_t xfrag, const size_t yfrag,
    const string& texName)
: MeshComponent(actor, texName, device){
    constexpr float scale=2.0f;

    const float dt=XM_2PI/xfrag;
    const float dy=1.0f/yfrag;
    vector<Vertex> vertices;
    vertices.reserve(xfrag*(yfrag+1));

    for(float y=0.0f; y<=1.0f; y+=dy){
        for(float t=0.0f; t<XM_2PI; t+=dt){
            const Matrix transform=Matrix::CreateRotationY(t);
            const float uv_x=t/XM_PI;

            vertices.emplace_back(Vertex{
                .position=scale*Vector3::Transform({1.0f, y-0.5f, 0.0f}, transform),
                .normal=Vector3::TransformNormal({1.0f, 0.0f, 0.0f}, transform),
                .uv{uv_x>1.0f ? uv_x-1.0f : uv_x, y}
            });
        }
    }

    vector<Polygon> polygons;
    polygons.reserve(2*xfrag*yfrag);

    for(size_t j=0; j<yfrag; ++j){
        const size_t j1=j+1;
        for(size_t i=0; i<xfrag; ++i){
            const size_t i1=(i+1)%xfrag;

            const size_t bottomLeft =xfrag* j+ i;
            const size_t bottomRight=xfrag* j+i1;
            const size_t topLeft    =xfrag*j1+ i;
            const size_t topRight   =xfrag*j1+i1;

            polygons.emplace_back(Polygon{topLeft, bottomRight, topRight});
            polygons.emplace_back(Polygon{bottomRight, topLeft, bottomLeft});
        }
    }

    mesh.verticies=std::move(vertices);
    mesh.polygons=std::move(polygons);
}

CylinderMeshComponent::~CylinderMeshComponent()=default;
