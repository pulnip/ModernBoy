#include <string>
#include <utility>
#include <vector>
#include "backends/metal/mesh.hpp"
#include "app_state.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Metal;

static std::vector<float> toFloats(const Vertices& vertices);

Mesh::Mesh(Mesh&& other){ moveFrom(std::move(other)); }
Mesh& Mesh::operator=(Mesh&& other){
    moveFrom(std::move(other));
    return *this;
}
void Mesh::moveFrom(Mesh&& other){
    meshPtr = other.meshPtr;
    other.meshPtr = nullptr;
}

Mesh::Mesh(RawMesh& rawMesh, AppState& app){
    auto vertices = toFloats(rawMesh.vertices);
    const char* texPath = nullptr;
    if(!rawMesh.textures.empty()){
        texPath = rawMesh.textures[0].c_str();
    }

    meshPtr = createMesh(app.renderer.context.metalLayer,
        vertices.data(), vertices.size() / 8,
        rawMesh.indices.data(), rawMesh.indices.size(),
        texPath
    );
}

static std::vector<float> toFloats(const Vertices& vertices){
    std::vector<float> result(8*vertices.size());

    for(size_t i=0; i<vertices.size(); ++i){
        size_t base = 8*i;
        result[base+0] = vertices[i].position[0];
        result[base+1] = vertices[i].position[1];
        result[base+2] = vertices[i].position[2];
        result[base+3] = vertices[i].normal[0];
        result[base+4] = vertices[i].normal[1];
        result[base+5] = vertices[i].normal[2];
        result[base+6] = vertices[i].texcoord[0];
        result[base+7] = vertices[i].texcoord[1];
    }

    return result;
}
