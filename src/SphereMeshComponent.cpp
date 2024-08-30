#include <directxmath/DirectXMath.h>
#include "SphereMeshComponent.hpp"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace DirectX::Colors;
using namespace ModernBoy;

SphereMeshComponent::SphereMeshComponent(Actor& actor,
    const ComPtr<ID3D11Device>& device,
    const size_t xfrag, const size_t yfrag,
    const string& texName)
: MeshComponent(actor, texName, device){
    constexpr float scale=2.0f;

    const float dp=XM_2PI/xfrag;
    const float dt=XM_PI/yfrag;
    vector<Vertex> vertices;
    vertices.reserve(xfrag*(yfrag+1));

    for(float t=-XM_PIDIV2; t<=XM_PIDIV2; t+=dt){
        for(float p=0.0f; p<XM_2PI; p+=dp){
            const Matrix transform=Matrix::CreateFromYawPitchRoll(p, 0.0f, t);
            const float uv_x=p/XM_2PI;
            const float uv_y=0.5f-t/XM_PI;

            vertices.emplace_back(Vertex{
                .position=scale*Vector3::Transform({1.0f, 0.0f, 0.0f}, transform),
                .normal=Vector3::TransformNormal({1.0f, 0.0f, 0.0f}, transform),
                .uv{uv_x, uv_y}
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

SphereMeshComponent::~SphereMeshComponent()=default;
