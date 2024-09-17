#include "GridMeshComponent.hpp"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace DirectX::Colors;
using namespace ModernBoy;

GridMeshComponent::GridMeshComponent(Actor& actor,
    const ComPtr<Device>& device,
    const size_t xfrag, const size_t yfrag,
    const string& texName)
: MeshComponent(actor, texName, device){
    constexpr float scale=2.0f;

    const float dx=1.0f/xfrag;
    const float dy=1.0f/yfrag;
    vector<Vertex> vertices;
    vertices.reserve((xfrag+1)*(yfrag+1));

    for(float y=0.0f; y<=1.0f; y+=dy){
        for(float x=0.0f; x<=1.0f; x+=dx){
            vertices.emplace_back(Vertex{
                .position=scale*Vector3(x-0.5f, y-0.5f, 0.0f),
                .normal{0.0f, 0.0f, -1.0f},
                .uv{x, y}
            });
        }
    }

    vector<Polygon> polygons;
    polygons.reserve(2*xfrag*yfrag);

    const float nx=xfrag+1;
    for(size_t j=0; j<yfrag; ++j){
        for(size_t i=0; i<xfrag; ++i){
            const size_t j1=j+1;
            const size_t i1=i+1;

            const size_t bottomLeft =nx* j+ i;
            const size_t bottomRight=nx* j+i1;
            const size_t topLeft    =nx*j1+ i;
            const size_t topRight   =nx*j1+i1;

            polygons.emplace_back(Polygon{topLeft, topRight, bottomRight});
            polygons.emplace_back(Polygon{bottomRight, bottomLeft, topLeft});
        }
    }

    mesh.verticies=std::move(vertices);
    mesh.polygons=std::move(polygons);
}

GridMeshComponent::~GridMeshComponent()=default;
