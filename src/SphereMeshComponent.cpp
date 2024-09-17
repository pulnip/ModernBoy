#include <directxmath/DirectXMath.h>
#include "SphereMeshComponent.hpp"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace DirectX::Colors;
using namespace ModernBoy;

SphereMeshComponent::SphereMeshComponent(Actor& actor,
    const ComPtr<Device>& device,
    const size_t xfrag, const size_t yfrag,
    const string& texName)
: MeshComponent(actor, texName, device){
    constexpr float scale=2.0f;

    vector<Vertex> vertices;
    vertices.reserve(xfrag*(yfrag+1));

    for(size_t y=0; y<=yfrag; ++y){
        for(size_t i=0; i<=xfrag; ++i){
            const float uv_x=static_cast<float>(i)/xfrag;
            const float uv_y=static_cast<float>(y)/yfrag;

            const Matrix transform=Matrix::CreateFromYawPitchRoll(
                XM_2PI*(1.0f-uv_x), XM_PI*(1.0f-uv_y) - XM_PIDIV2, 0.0f
            );

            vertices.emplace_back(Vertex{
                .position=Vector3::Transform({0.0f, 0.0f, -scale}, transform),
                .normal=Vector3::TransformNormal({0.0f, 0.0f, -1.0f}, transform),
                .uv{uv_x, uv_y}
            });
        }
    }

    vector<Polygon> polygons;
    polygons.reserve(2*xfrag*yfrag);

    const size_t width=xfrag+1;

    for(size_t j=0; j<yfrag; ++j){
        const size_t j1=j+1;
        for(size_t i=0; i<xfrag; ++i){
            const size_t i1=i+1;

            const size_t bottomLeft =width* j+ i;
            const size_t bottomRight=width* j+i1;
            const size_t topLeft    =width*j1+ i;
            const size_t topRight   =width*j1+i1;

            polygons.emplace_back(Polygon{topLeft, bottomRight, topRight});
            polygons.emplace_back(Polygon{bottomRight, topLeft, bottomLeft});
        }
    }

    mesh.verticies=std::move(vertices);
    mesh.polygons=std::move(polygons);
}

SphereMeshComponent::~SphereMeshComponent()=default;
