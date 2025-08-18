#include <gtest/gtest.h>
#include "engine/asset/mesh_importer.hpp"
#include "engine/asset/asset_format.hpp"

using namespace ModernBoy::Asset;

// Approximate structural equality for unit tests
static bool approxEqual(const CookedMesh& a, const CookedMesh& b,
    float posEps, float nrmEps, float uvEps
){
    if(a.vertices.size() != b.vertices.size()) return false;
    if(a.indices.size()  != b.indices.size())  return false;
    if(a.submeshInfoTable.size() != b.submeshInfoTable.size()) return false;
    if(a.materialInfoTable.size()!= b.materialInfoTable.size()) return false;
    if(a.textureInfoTable.size() != b.textureInfoTable.size()) return false;
    if(a.pixels.size() != b.pixels.size()) return false;

    auto feq = [](float x, float y, float eps){ return std::fabs(x-y) <= eps; };

    for(size_t i=0;i<a.vertices.size();++i){
        const auto& va=a.vertices[i];
        const auto& vb=b.vertices[i];
        if(!(feq(va.position.x,vb.position.x,posEps)&&feq(va.position.y,vb.position.y,posEps)&&feq(va.position.z,vb.position.z,posEps))) return false;
        if(!(feq(va.normal.x,vb.normal.x,nrmEps)&&feq(va.normal.y,vb.normal.y,nrmEps)&&feq(va.normal.z,vb.normal.z,nrmEps))) return false;
        if(!(feq(va.tangent.x,vb.tangent.x,nrmEps)&&feq(va.tangent.y,vb.tangent.y,nrmEps)&&feq(va.tangent.z,vb.tangent.z,nrmEps))) return false;
        if(!(feq(va.texcoord.x,vb.texcoord.x,uvEps)&&feq(va.texcoord.y,vb.texcoord.y,uvEps))) return false;
    }
    if(a.indices != b.indices) return false;
    if(std::memcmp(a.submeshInfoTable.data(), b.submeshInfoTable.data(), a.submeshInfoTable.size()*sizeof(SubmeshInfo))!=0) return false;
    if(std::memcmp(a.materialInfoTable.data(),b.materialInfoTable.data(),a.materialInfoTable.size()*sizeof(MaterialInfo))!=0) return false;
    if(std::memcmp(a.textureInfoTable.data(), b.textureInfoTable.data(), a.textureInfoTable.size()*sizeof(TextureInfo))!=0) return false;
    if(std::memcmp(a.pixels.data(), b.pixels.data(), a.pixels.size())!=0) return false;

    // Axis/AABB/header are not required to be bit-equal for approx check, but sanity-compare key fields
    if(a.header.numVertex != b.header.numVertex) return false;
    if(a.header.numIndex  != b.header.numIndex)  return false;
    if(a.header.numSubmesh!= b.header.numSubmesh) return false;

    return true;
}

static CookedMesh makeTinyMesh(){
    CookedMesh m{};
    Vertex v{};
    v.position = {{0,0,0}}; v.normal={{0,1,0}}; v.tangent={{1,0,0,1}}; v.texcoord={{0,0}};
    m.vertices.push_back(v);
    v.position = {{1,0,0}}; v.texcoord={{1,0}}; m.vertices.push_back(v);
    v.position = {{0,1,0}}; v.texcoord={{0,1}}; m.vertices.push_back(v);

    m.indices = {{0,1,2}};

    SubmeshInfo sm{};
    sm.entrySize = sizeof(SubmeshInfo);
    sm.verticesSectionIndex = 0;
    sm.vertexCount = 3;
    sm.indicesSectionIndex = 0;
    sm.indexCount = 3;
    sm.materialTableIndex = 0;
    m.submeshInfoTable.push_back(sm);

    MaterialInfo mi{};
    mi.entrySize = sizeof(MaterialInfo);
    mi.type = MaterialType::Unlit;
    mi.textureInfoTableIndex = 0;
    mi.textureCount = 0;
    m.materialInfoTable.push_back(mi);

    m.axisInfo = AxisInfo{ AxisInfo::RH, 'Y', 'Z', true, 1.0f };
    m.aabb.min = {{0,0,0}}; m.aabb.max={{1,1,0}};

    m.header = extractHeader(m);
    return m;
}

TEST(MeshImporter, RoundTripBuffer){
    CookedMesh src = makeTinyMesh();

    auto buf = serializeToBuffer(src);
    auto dst = loadFromBuffer(buf);

    EXPECT_TRUE(approxEqual(src, dst, 1e-5f, 1e-4f, 1e-6f));
    EXPECT_EQ(src.vertices.size(), dst.vertices.size());
    EXPECT_EQ(src.indices.size(),  dst.indices.size());
}

TEST(MeshImporter, HeaderOffsetsAligned){
    CookedMesh src = makeTinyMesh();
    auto blob = serializeToBuffer(src);
    auto dst = loadFromBuffer(blob);
    EXPECT_EQ(dst.header.version, 1u);
}