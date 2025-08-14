#include <fstream>
#include <functional>
#include <memory>
#include <print>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include "engine/asset/mesh_importer.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Asset;

namespace fs = std::filesystem;

struct RawScene{
    std::unique_ptr<aiScene, void(*)(aiScene*)> scene{
        nullptr, [](aiScene* s){
            if(s) aiReleaseImport(s); 
        }
    };
};

static auto import(const fs::path& inputPath)->RawScene;
static auto extractAxisInfo(const RawScene&)->AxisInfo;
static auto buildTransform(
    const AxisInfo& src, const AxisInfo& dst)->Mat4;
static auto buildMesh(const RawScene&,
    const Mat4&, bool flipV)->CookedMesh;

auto Asset::importModelFile(const fs::path& inputPath,
    const CookOptions& options
)->CookedMesh{
    auto rawScene = import(inputPath);

    auto axisInfo = extractAxisInfo(rawScene);
    Mat4 mat = buildTransform(axisInfo, options.axes);

    return buildMesh(rawScene, mat, options.axes.flipV);
}

void Asset::serialize(const CookedMesh& cooked, const fs::path& outputPath){
    std::ofstream ofs(outputPath, std::ios::binary);
    if(!ofs)
        throw std::runtime_error("failed to open output file");

    Header header = cooked.header; // start from defaults
    const char magic[]="MBMESH\1";
    std::memcpy(header.magic, magic, 8);

    header.version       = 1;
    header.numVertex   = static_cast<uint32_t>(cooked.vertices.size());
    header.numIndex    = static_cast<uint32_t>(cooked.indices.size());
    header.numSubmesh  = static_cast<uint32_t>(cooked.submeshInfoTable.size());
    header.numMaterial = static_cast<uint32_t>(cooked.materialInfoTable.size());
    header.verticesSectionStride  = static_cast<uint32_t>(sizeof(Vertex));
    header.indicesSectionStride   = static_cast<uint32_t>(sizeof(uint32_t));

    ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
    if(!cooked.vertices.empty()) 
        ofs.write(reinterpret_cast<const char*>(cooked.vertices.data()),
            cooked.vertices.size()*sizeof(Vertex));
    if(!cooked.indices.empty())
        ofs.write(reinterpret_cast<const char*>(cooked.indices.data()),
            cooked.indices.size()*sizeof(uint32_t));
    if(!cooked.submeshInfoTable.empty())
        ofs.write(reinterpret_cast<const char*>(cooked.submeshInfoTable.data()),
            cooked.submeshInfoTable.size()*sizeof(SubmeshInfo));
}

static auto import(const fs::path& inputPath)->RawScene{
    Assimp::Importer importer;
    RawScene rawScene;
    constexpr unsigned flags =
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_ImproveCacheLocality |
        aiProcess_SortByPType |
        aiProcess_ValidateDataStructure;

    const aiScene* loaded = importer.ReadFile(
        inputPath.string(), flags);
    if(!loaded){
        throw std::runtime_error(std::string{
            "Assimp failed: "} + importer.GetErrorString()
        );
    }
    aiScene* orphaned = importer.GetOrphanedScene();
    rawScene.scene.reset(orphaned);
    return rawScene;
}

static auto extractAxisInfo(const RawScene& scene)->AxisInfo{
    // ToDo. extract real axis info from scene
    return AxisInfo{
        .hand = AxisInfo::Hand::RH,
        .up = 'Y',
        .forward = 'Z',
        .flipV = true,
        .unit = 1.0f
    };
}

static auto buildTransform(
    const AxisInfo& src, const AxisInfo& dst
)->Mat4{
    auto mat = unitMat();

    const float scale = (src.unit <= 0.f ? 1.f : src.unit) / (dst.unit <= 0.f ? 1.f : dst.unit);

    mat[0][0] *= scale;
    mat[1][1] *= scale;
    mat[2][2] *= scale;

    if(src.hand==AxisInfo::RH && dst.hand==AxisInfo::LH)
        mat[0][0] *= -1.0f;

    return mat;
}

static auto toAi(const Mat4& mat){
    return aiMatrix4x4(
        mat[0][0], mat[0][1], mat[0][2], mat[0][3],
        mat[1][0], mat[1][1], mat[1][2], mat[1][3],
        mat[2][0], mat[2][1], mat[2][2], mat[2][3],
        mat[3][0], mat[3][1], mat[3][2], mat[3][3]
    );
}
static float Det3x3(const aiMatrix3x3& aiMat){
    return aiMat.a1*(aiMat.b2*aiMat.c3 - aiMat.b3*aiMat.c2)
         - aiMat.a2*(aiMat.b1*aiMat.c3 - aiMat.b3*aiMat.c1)
         + aiMat.a3*(aiMat.b1*aiMat.c2 - aiMat.b2*aiMat.c1);
}

static void computeAABB(CookedMesh& mesh){
    for(const auto& v: mesh.vertices){
        mesh.aabb.min.x = std::min(mesh.aabb.min.x, v.position.x);
        mesh.aabb.min.y = std::min(mesh.aabb.min.y, v.position.y);
        mesh.aabb.min.z = std::min(mesh.aabb.min.z, v.position.z);

        mesh.aabb.min.x = std::max(mesh.aabb.min.x, v.position.x);
        mesh.aabb.min.y = std::max(mesh.aabb.min.y, v.position.y);
        mesh.aabb.min.z = std::max(mesh.aabb.min.z, v.position.z);
    }
}

static auto buildMesh(const RawScene& scene,
    const Mat4& mat, bool flipV
)->CookedMesh{
    auto aiScene = scene.scene.get();
    CookedMesh out{};
    if(!scene.scene || !scene.scene->mRootNode)
        return out;

    const aiMatrix4x4 aiMat = toAi(mat);

    if(aiScene->HasMaterials()){
        out.materialInfoTable.resize(aiScene->mNumMaterials);
    }

    std::function<void(const aiNode*, aiMatrix4x4)> visit = [&](const aiNode* node, aiMatrix4x4 parent){
        aiMatrix4x4 global = parent * node->mTransformation;
        aiMatrix4x4 T = aiMat * global;

        aiMatrix3x3 T3(T);
        aiMatrix3x3 N3 = T3; N3.Inverse(); N3.Transpose();
        const float detT = Det3x3(T3);
        const bool flipWinding = (detT < 0.0f);

        for(unsigned im=0; im<node->mNumMeshes; ++im){
            const aiMesh* m = aiScene->mMeshes[node->mMeshes[im]];
            const uint32_t baseVertex = (uint32_t)out.vertices.size();
            const uint32_t baseIndex  = (uint32_t)out.indices.size();

            // vertices
            for(unsigned v=0; v<m->mNumVertices; ++v){
                Vertex vx{};
                if(m->HasPositions()){
                    aiVector3D p = T * m->mVertices[v];
                    for(int i=0; i<3; ++i)
                        vx.position[i] = p[i];
                }
                if(m->HasNormals()){
                    aiVector3D n = N3 * m->mNormals[v]; n.Normalize();
                    for(int i=0; i<3; ++i)
                        vx.position[i] = n[i];
                }
                if(m->HasTangentsAndBitangents()){
                    aiVector3D t = N3 * m->mTangents[v]; t.Normalize();
                    for(int i=0; i<3; ++i)
                        vx.position[i] = t[i];
                }
                if(m->HasTextureCoords(0)){
                    vx.texcoord.x = m->mTextureCoords[0][v].x;
                    vx.texcoord.y = flipV ?
                        (1.0f - m->mTextureCoords[0][v].y) :
                        m->mTextureCoords[0][v].y;
                }
                out.vertices.push_back(vx);
            }

            // indices (triangulated)
            for(unsigned f=0; f<m->mNumFaces; ++f){
                const aiFace& face = m->mFaces[f];
                if(face.mNumIndices == 3){
                    uint32_t i0 = baseVertex + face.mIndices[0];
                    uint32_t i1 = baseVertex + face.mIndices[1];
                    uint32_t i2 = baseVertex + face.mIndices[2];
                    if(flipWinding){
                        out.indices.push_back(i0);
                        out.indices.push_back(i2);
                        out.indices.push_back(i1);
                    }else{
                        out.indices.push_back(i0);
                        out.indices.push_back(i1);
                        out.indices.push_back(i2);
                    }
                }
            }
            SubmeshInfo sm{};

            sm.indicesSectionIndex = baseIndex;
            sm.indexCount = (uint32_t)out.indices.size() - sm.indicesSectionIndex;
            sm.materialTableIndex = m->mMaterialIndex;
            out.submeshInfoTable.push_back(sm);
        }

        for(unsigned c=0; c<node->mNumChildren; ++c){
            visit(node->mChildren[c], global);
        }
    };

    visit(aiScene->mRootNode, aiMatrix4x4());

    out.header.numVertex = (uint32_t)out.vertices.size();
    out.header.numIndex = (uint32_t)out.indices.size();
    out.header.numSubmesh = (uint32_t)out.submeshInfoTable.size();
    out.header.numMaterial = (uint32_t)out.materialInfoTable.size();

    computeAABB(out);
    return out;
}