#include <algorithm>
#include <cmath>
#include <cstring>
#include <format>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <numbers>
#include <string>
#include <print>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <iterator>
#include "engine/asset/mesh_importer.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Asset;

namespace fs = std::filesystem;

namespace {
    EmbeddedMesh createEmbeddedTriangle(){
        std::vector<Vertex> vertices = {
            {
                {{0.0f, 1.0f, 0.0f}}, {{0.0f, 0.0f, -1.0f}},
                {{0.5f, 0.0f}}, {{0.0f, -1.0f, 0.0f, -1.0f}}
            },{
                {{-1.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f, -1.0f}},
                {{0.0f, 1.0f}}, {{0.0f, -1.0f, 0.0f, -1.0f,}}
            },{
                {{1.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f, -1.0f}},
                {{1.0f, 1.0f}}, {{0.0f, -1.0f, 0.0f, -1.0f}}
            }
        };
        Indices indices = {1, 0, 2};
    
        return {vertices, indices};
    }

    EmbeddedMesh createEmbeddedRectangle(){
        std::vector<Vertex> vertices = {
            {
                {{-0.5f, -0.5f, 0.0f}}, {{0.0f, 0.0f, -1.0f}}, 
                {{0.0f, 1.0f}}, {{0.0f, -1.0f, 0.0f, -1.0f}}
            },{
                {{0.5f, -0.5f, 0.0f}}, {{0.0f, 0.0f, -1.0f}},
                {{1.0f, 1.0f}}, {{0.0f, -1.0f, 0.0f, -1.0f}}
            },{
                {{0.5f, 0.5f, 0.0f}}, {{0.0f, 0.0f, -1.0f}},
                {{1.0f, 0.0f}}, {{0.0f, -1.0f, 0.0f, -1.0f}}
            },{
                {{-0.5f, 0.5f, 0.0f}}, {{0.0f, 0.0f, -1.0f}},
                {{0.0f, 0.0f}}, {{0.0f, -1.0f, 0.0f, -1.0f}}
            }
        };
        Indices indices = {
            0, 2, 1,
            0, 3, 2
        };

        return EmbeddedMesh{vertices, indices};
    }
    EmbeddedMesh createEmbeddedCube(){
        std::vector<Vertex> vertices = {
            // front
            {
                {{-0.5f, -0.5f, -0.5f}}, {{0.0f, 0.0f, -1.0f}},
                {{0.0f, 1.0f}}, {},
            },{
                {{ 0.5f, -0.5f, -0.5f}}, {{0.0f, 0.0f, -1.0f}},
                {{1.0f, 1.0f}}, {}
            },{
                {{ 0.5f,  0.5f, -0.5f}}, {{0.0f, 0.0f, -1.0f}},
                {{1.0f, 0.0f}}, {}
            },{
                {{-0.5f,  0.5f, -0.5f}}, {{0.0f, 0.0f, -1.0f}},
                {{0.0f, 0.0f}}, {}
            },
            // back
            {
                {{-0.5f, -0.5f,  0.5f}}, {{0.0f, 0.0f, 1.0f}},
                {{0.0f, 1.0f}}, {}
            },{
                {{ 0.5f, -0.5f,  0.5f}}, {{0.0f, 0.0f, 1.0f}},
                {{1.0f, 1.0f}}, {}
            },{
                {{ 0.5f,  0.5f,  0.5f}}, {{0.0f, 0.0f, 1.0f}},
                {{1.0f, 0.0f}}, {}
            },{
                {{-0.5f,  0.5f,  0.5f}}, {{0.0f, 0.0f, 1.0f}},
                {{0.0f, 0.0f}}, {}
            },
            // left
            {
                {{-0.5f,  0.5f, -0.5f}}, {{-1.0f, 0.0f, 0.0f}},
                {{0.0f, 1.0f}}, {}
            },{
                {{-0.5f, -0.5f, -0.5f}}, {{-1.0f, 0.0f, 0.0f}},
                {{1.0f, 1.0f}}, {}
            },{
                {{-0.5f, -0.5f,  0.5f}}, {{-1.0f, 0.0f, 0.0f}},
                {{1.0f, 0.0f}}, {}
            },{
                {{-0.5f,  0.5f,  0.5f}}, {{-1.0f, 0.0f, 0.0f}},
                {{0.0f, 0.0f}}, {}
            },
            // right
            {
                {{ 0.5f,  0.5f, -0.5f}}, {{1.0f, 0.0f, 0.0f}},
                {{0.0f, 1.0f}}, {}
            },{
                {{ 0.5f, -0.5f, -0.5f}}, {{1.0f, 0.0f, 0.0f}},
                {{1.0f, 1.0f}}, {}
            },{
                {{ 0.5f, -0.5f,  0.5f}}, {{1.0f, 0.0f, 0.0f}},
                {{1.0f, 0.0f}}, {}
            },{
                {{ 0.5f,  0.5f,  0.5f}}, {{1.0f, 0.0f, 0.0f}},
                {{0.0f, 0.0f}}, {}
            },
            // bottom
            {
                {{-0.5f, -0.5f, -0.5f}}, {{0.0f, -1.0f, 0.0f}},
                {{0.0f, 1.0f}}, {}
            },{
                {{ 0.5f, -0.5f, -0.5f}}, {{0.0f, -1.0f, 0.0f}},
                {{1.0f, 1.0f}}, {}
            },{
                {{ 0.5f, -0.5f,  0.5f}}, {{0.0f, -1.0f, 0.0f}},
                {{1.0f, 0.0f}}, {}
            },{
                {{-0.5f, -0.5f,  0.5f}}, {{0.0f, -1.0f, 0.0f}},
                {{0.0f, 0.0f}}, {}
            },
            // top
            {
                {{-0.5f,  0.5f, -0.5f}}, {{0.0f, 1.0f, 0.0f}},
                {{0.0f, 1.0f}}, {}
            },{
                {{ 0.5f,  0.5f, -0.5f}}, {{0.0f, 1.0f, 0.0f}},
                {{1.0f, 1.0f}}, {}
            },{
                {{ 0.5f,  0.5f,  0.5f}}, {{0.0f, 1.0f, 0.0f}},
                {{1.0f, 0.0f}}, {}
            },{
                {{-0.5f,  0.5f,  0.5f}}, {{0.0f, 1.0f, 0.0f}},
                {{0.0f, 0.0f}}, {}
            }
        };
        Indices indices = {
            // front
             2,  0,  3,
             1,  0,  2,
            // back
             4,  5,  6,
             6,  7,  4,
            // left
            11,  8,  9,
             9, 10, 11,
            // right
            13, 12, 14,
            15, 14, 12,
            // bottom
            16, 17, 18,
            18, 19, 16,
            // top
            21, 20, 22,
            22, 20, 23
        };

        return EmbeddedMesh{vertices, indices};
    }
    EmbeddedMesh createEmbeddedSphere(float radius=1.0f,
        int numSlices=32, int numStacks=16
    ){
        std::vector<Vertex> vertices;
        Indices indices;

        const float dTheta = 2 * std::numbers::pi / numSlices;
        const float dPhi = std::numbers::pi / numStacks;

        for(int i=0; i<=numStacks; ++i){
            const auto y = radius * cos(dPhi * i);
            const auto rad = radius * sin(dPhi * i);
            const auto v = static_cast<float>(i) / numStacks;
            for(int j=0; j<=numSlices; ++j){
                const auto x = rad * cos(dTheta * j);
                const auto z = rad * sin(dTheta * j);
                const auto u = static_cast<float>(j)/numSlices;

                vertices.emplace_back(Vertex{
                    {{x, y, z}}, {{x, y, z}},
                    {{u, v}}, {}
                });
            }
        }
        for(int i=0; i<numStacks; ++i){
            const auto base = (numSlices+1) * i;
            for(int j=0; j<numSlices; ++j){
                const uint32_t topLeft = base + j;
                const uint32_t topRight = base + (j+1);
                const uint32_t bottomLeft = base+(numSlices+1) + j;
                const uint32_t bottomRight = base+(numSlices+1) + (j+1);
                Indices rect{
                    topLeft, topRight, bottomRight,
                    topLeft, bottomRight, bottomLeft
                };
                indices.append_range(rect);
            }
        }

        return EmbeddedMesh{vertices, indices};
    }

    struct RawScene{
        std::unique_ptr<aiScene, void(*)(aiScene*)> scene{
            nullptr, [](aiScene* s){
                if(s) aiReleaseImport(s);
            }
        };
        std::filesystem::path baseDir;
    };

    TextureUsage mapTextureUsage(aiTextureType t){
        switch(t){
            case aiTextureType_BASE_COLOR:
            case aiTextureType_DIFFUSE: return TextureUsage::BaseColor;
            case aiTextureType_NORMALS:
            case aiTextureType_NORMAL_CAMERA: return TextureUsage::Normal;
            case aiTextureType_METALNESS:
            case aiTextureType_DIFFUSE_ROUGHNESS:
            case aiTextureType_UNKNOWN: return TextureUsage::MR;
            case aiTextureType_EMISSIVE: return TextureUsage::Emissive;
            default: return TextureUsage::BaseColor;
        }
    }

    const char* usageToSlotName(TextureUsage u){
        switch(u){
        case TextureUsage::BaseColor: return "BaseColor";
        case TextureUsage::Normal:    return "Normal";
        case TextureUsage::MR:        return "MR";
        case TextureUsage::Emissive:  return "Emissive";
        default: return "Unknown";
        }
    }

    auto import(const fs::path& inputPath){
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
            throw std::runtime_error(
                std::string("Assimp failed: ") +
                std::string(importer.GetErrorString())
            );
        }
        aiScene* orphaned = importer.GetOrphanedScene();
        rawScene.scene.reset(orphaned);
        rawScene.baseDir = inputPath.parent_path();
        return rawScene;
    }

    auto extractAxisInfo(const RawScene& scene)->AxisInfo{
        // ToDo. extract real axis info from scene
        return AxisInfo{
            .hand = AxisInfo::Hand::RH,
            .up = 'Y',
            .forward = 'Z',
            .flipV = true,
            .unit = 1.0f
        };
    }

    auto buildTransform(
        const AxisInfo& src,
        const AxisInfo& dst
    ){
        auto mat = unitMat();

        const float scale = (src.unit <= 0.f ? 1.f : src.unit) / (dst.unit <= 0.f ? 1.f : dst.unit);

        mat[0][0] *= scale;
        mat[1][1] *= scale;
        mat[2][2] *= scale;

        if(src.hand==AxisInfo::RH && dst.hand==AxisInfo::LH)
            mat[0][0] *= -1.0f;

        return mat;
    }

    auto toAi(const Mat4& mat){
        return aiMatrix4x4(
            mat[0][0], mat[0][1], mat[0][2], mat[0][3],
            mat[1][0], mat[1][1], mat[1][2], mat[1][3],
            mat[2][0], mat[2][1], mat[2][2], mat[2][3],
            mat[3][0], mat[3][1], mat[3][2], mat[3][3]
        );
    }
    auto Det3x3(const aiMatrix3x3& aiMat){
        return aiMat.a1*(aiMat.b2*aiMat.c3 - aiMat.b3*aiMat.c2)
             - aiMat.a2*(aiMat.b1*aiMat.c3 - aiMat.b3*aiMat.c1)
             + aiMat.a3*(aiMat.b1*aiMat.c2 - aiMat.b2*aiMat.c1);
    }

    auto computeAABB(const CookedMesh& mesh){
        AABB aabb{};
        bool inited = false;
        for(const auto& sm : mesh.submeshes){
            for(const auto& v : sm.vertices){
                if(!inited){ aabb.min = aabb.max = v.position; inited = true; }
                aabb.min.x = std::min(aabb.min.x, v.position.x);
                aabb.min.y = std::min(aabb.min.y, v.position.y);
                aabb.min.z = std::min(aabb.min.z, v.position.z);
                aabb.max.x = std::max(aabb.max.x, v.position.x);
                aabb.max.y = std::max(aabb.max.y, v.position.y);
                aabb.max.z = std::max(aabb.max.z, v.position.z);
            }
        }
        return aabb;
    }

    auto buildMesh(
        const RawScene& scene,
        const AxisInfo& dstAxisInfo
    )->CookedMesh{
        auto aiScene = scene.scene.get();
        CookedMesh out{};
        if(!scene.scene || !scene.scene->mRootNode)
            return out;

        auto srcAxisInfo = extractAxisInfo(scene);
        Mat4 mat = buildTransform(srcAxisInfo, dstAxisInfo);
        const aiMatrix4x4 aiMat = toAi(mat);

        // 1) Build CookedMaterial map from aiMaterials (URI-only, no embedded textures)
        if(aiScene->HasMaterials()){
            for(unsigned i=0; i<aiScene->mNumMaterials; ++i){
                const aiMaterial* m = aiScene->mMaterials[i];

                CookedMaterial cm{};
                aiString aiName = m->GetName();
                cm.name = (aiName.length>0) ? std::string(aiName.C_Str())
                                            : std::string(std::format("Material{}", i));
                cm.type = MaterialType::Unlit; // ToDo. will upgrade to PBR if any textures attached

                auto appendUris = [&](aiTextureType t){
                    TextureUsage usage = mapTextureUsage(t);
                    unsigned texCount = m->GetTextureCount(t);
                    for(unsigned ti=0; ti<texCount; ++ti){
                        aiString texPath; if(m->GetTexture(t, ti, &texPath) != aiReturn_SUCCESS) continue;
                        std::string uri;
                        if(texPath.length>0 && texPath.C_Str()[0]=='*'){
                            // Embedded; mark with pseudo-URI (to be resolved by cooker/packager)
                            int embeddedIndex = std::atoi(texPath.C_Str()+1);
                            uri = std::format("embedded:{}", embeddedIndex);
                        } else {
                            fs::path full = scene.baseDir / fs::path(texPath.C_Str());
                            uri = full.lexically_normal().string();
                        }
                        CookedTexture ct{};
                        ct.usage = usage;
                        ct.flags = (usage==TextureUsage::BaseColor || usage==TextureUsage::Emissive)
                                    ? TextureFlag_SRGB : 0;
                        ct.uri = std::move(uri);
                        // store by human-friendly slot key; if multiple, keep the first
                        const char* key = usageToSlotName(usage);
                        if(!cm.textures.contains(key)) cm.textures.emplace(key, std::move(ct));
                    }
                };

                // Common PBR slots
                appendUris(aiTextureType_BASE_COLOR);
                appendUris(aiTextureType_DIFFUSE);
                appendUris(aiTextureType_NORMALS);
                appendUris(aiTextureType_NORMAL_CAMERA);
                appendUris(aiTextureType_METALNESS);
                appendUris(aiTextureType_DIFFUSE_ROUGHNESS);
                appendUris(aiTextureType_UNKNOWN); // some tools pack ORM here
                appendUris(aiTextureType_EMISSIVE);

                if(!cm.textures.empty()) cm.type = MaterialType::PBR;

                // For middle-form convenience, key materials by their name
                out.materials.emplace(cm.name, std::move(cm));
            }
        }

        // 2) Traverse nodes, create CookedSubmesh with local vertices/indices
        std::function<void(const aiNode*, aiMatrix4x4)> visit = [&](const aiNode* node, aiMatrix4x4 parent){
            aiMatrix4x4 global = parent * node->mTransformation;
            aiMatrix4x4 T = aiMat * global;
            aiMatrix3x3 T3(T); aiMatrix3x3 N3 = T3; N3.Inverse(); N3.Transpose();
            const bool flipWinding = (Det3x3(T3) < 0.0f);

            for(unsigned im=0; im<node->mNumMeshes; ++im){
                const aiMesh* m = aiScene->mMeshes[node->mMeshes[im]];

                CookedSubmesh sm{};
                sm.primitiveType = PrimitiveType::TriangleList;
                // Slot name suggestion: NodeName#LocalMeshIndex (stable & readable)
                sm.materialSlotName = node->mName.length>0
                    ? std::format("{}#{}", node->mName.C_Str(), im)
                    : std::format("Submesh#{}", out.submeshes.size());

                sm.vertices.reserve(m->mNumVertices);
                for(unsigned v=0; v<m->mNumVertices; ++v){
                    Vertex vx{};
                    if(m->HasPositions()){
                        aiVector3D p = T * m->mVertices[v];
                        vx.position = {{p.x, p.y, p.z}};
                    }
                    if(m->HasNormals()){
                        aiVector3D n = N3 * m->mNormals[v]; n.Normalize();
                        vx.normal = {{n.x, n.y, n.z}};
                    }
                    if(m->HasTangentsAndBitangents()){
                        aiVector3D t = N3 * m->mTangents[v]; t.Normalize();
                        vx.tangent = {{t.x, t.y, t.z, -1.0f}}; // handedness placeholder
                    }
                    if(m->HasTextureCoords(0)){
                        vx.texcoord.x = m->mTextureCoords[0][v].x;
                        vx.texcoord.y = dstAxisInfo.flipV ? (1.0f - m->mTextureCoords[0][v].y)
                                                          :  m->mTextureCoords[0][v].y;
                    }
                    sm.vertices.push_back(vx);
                }

                // Local, 0-based indices
                sm.indices.reserve(m->mNumFaces * 3);
                for(unsigned f=0; f<m->mNumFaces; ++f){
                    const aiFace& face = m->mFaces[f];
                    if(face.mNumIndices == 3){
                        uint32_t i0 = face.mIndices[0];
                        uint32_t i1 = face.mIndices[1];
                        uint32_t i2 = face.mIndices[2];
                        if(flipWinding){ sm.indices.push_back(i0); sm.indices.push_back(i2); sm.indices.push_back(i1); }
                        else            { sm.indices.push_back(i0); sm.indices.push_back(i1); sm.indices.push_back(i2); }
                    }
                }

                out.submeshes.push_back(std::move(sm));
            }

            for(unsigned c=0; c<node->mNumChildren; ++c) visit(node->mChildren[c], global);
        };

        visit(aiScene->mRootNode, aiMatrix4x4());

        out.axisInfo = dstAxisInfo;
        out.aabb = computeAABB(out);

        return out;
    }
}

auto Asset::importMeshFile(const fs::path& inputPath,
    const CookOptions& options
)->CookedMesh{
    auto rawScene = import(inputPath);

    return buildMesh(rawScene, options.axes);
}

auto Asset::loadEmbeddedMesh(const std::string& name)->EmbeddedMesh{
    if(name.compare("triangle") == 0)
        return createEmbeddedTriangle();
    else if(name.compare("rectangle") == 0)
        return createEmbeddedRectangle();
    else if(name.compare("cube") == 0)
        return createEmbeddedCube();
    else if(name.compare("sphere") == 0)
        return createEmbeddedSphere();
    else
        throw std::runtime_error(
            std::format("Not Implemented Embedded Type: {}",
            name)
        );
}

// ===== Simple .mbmesh container (middle-form) I/O =====
// Binary layout (little-endian):
//   FileHeader{ magic="MBMESH\2", version=2, reserved }
//   AxisInfo, AABB
//   u32 submeshCount
//     repeat submeshCount times:
//       u32 primitiveType
//       u32 slotNameLen, bytes
//       u32 vertexCount, Vertex[vertexCount]
//       u32 indexCount,  u32[indexCount]
//   u32 materialCount
//     repeat materialCount times:
//       u32 nameLen, bytes
//       u32 materialType
//       u32 textureCount
//         repeat textureCount times:
//           u32 slotKeyLen, bytes (e.g., "BaseColor")
//           u32 usage
//           u16 flags; u16 pad
//           u32 uriLen, bytes
//
// This is a convenience container for tools. It is not the old table/offset format.

namespace {
#pragma pack(push,1)
struct FileHeaderV2 {
    char     magic[8];   // "MBMESH\2"
    uint32_t version;    // 2
    uint32_t reserved;   // 0
};
#pragma pack(pop)

// write helpers
template<class T>
static inline void wpod(std::vector<uint8_t>& buf, const T& v){
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&v);
    buf.insert(buf.end(), p, p+sizeof(T));
}
static inline void wbytes(std::vector<uint8_t>& buf, const void* data, size_t n){
    const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
    buf.insert(buf.end(), p, p+n);
}
static inline void wstr(std::vector<uint8_t>& buf, std::string_view s){
    uint32_t n = static_cast<uint32_t>(s.size());
    wpod(buf, n);
    if(n) wbytes(buf, s.data(), n);
}

// read helpers
struct R{
    const uint8_t* p;
    const uint8_t* e;

    inline bool ok() const { return p<=e; }

    template<class T>
    bool rpod(T& out){
        if(static_cast<size_t>(e-p) < sizeof(T)) return false;
        std::memcpy(&out, p, sizeof(T)); p += sizeof(T); return true;
    }
    bool rbytes(void* dst, size_t n){
        if(static_cast<size_t>(e-p) < n)
            return false;
        std::memcpy(dst,p,n); p+=n; return true;
    }
    bool rstr(std::string& out){
        uint32_t n = 0;
        if(!rpod(n))
            return false;
        if(static_cast<size_t>(e - p) < n)
            return false;
        out.assign(reinterpret_cast<const char*>(p), n);
        p += n;
        return true;
    }
};
}

auto Asset::extractHeader(const CookedMesh& cooked) -> Header {
    // Provide a best-effort synthetic header for legacy callers that still expect it.
    Header H{};
    const char magic[] = "MBMESH\2";
    std::memcpy(H.magic, magic, sizeof(H.magic));
    H.version = 0;
    H.headerSize = sizeof(Header);
    // We no longer use on-disk tables; fill some counts for reference only.
    H.numSubmesh = static_cast<uint32_t>(cooked.submeshes.size());
    H.numMaterial = static_cast<uint32_t>(cooked.materials.size());
    H.numTexture = 0; // unknown in this container; textures are per-material maps
    H.numVertex = 0;  // not used
    H.numIndex  = 0;  // not used

    return H;
}

auto Asset::serializeToBuffer(const CookedMesh& cooked) -> std::vector<uint8_t>{
    std::vector<uint8_t> buf;
    buf.reserve(1<<20);

    FileHeaderV2 FH{};
    std::memset(&FH,0,sizeof(FH));
    std::memcpy(FH.magic, "MBMESH\2", 8);
    FH.version = 0;
    FH.reserved = 0;
    wpod(buf, FH);

    // Axis + AABB
    wpod(buf, cooked.axisInfo);
    wpod(buf, cooked.aabb);

    // Submeshes
    uint32_t smCount = static_cast<uint32_t>(cooked.submeshes.size());
    wpod(buf, smCount);

    for(const auto& sm : cooked.submeshes){
        uint32_t prim = static_cast<uint32_t>(sm.primitiveType);
        wpod(buf, prim);
        wstr(buf, sm.materialSlotName);

        uint32_t vc = static_cast<uint32_t>(sm.vertices.size());
        uint32_t ic = static_cast<uint32_t>(sm.indices.size());
        wpod(buf, vc); if(vc) wbytes(buf, sm.vertices.data(), vc*sizeof(Vertex));
        wpod(buf, ic); if(ic) wbytes(buf, sm.indices.data(),  ic*sizeof(uint32_t));
    }

    // Materials (unordered_map -> write size, then each entry)
    uint32_t matCount = static_cast<uint32_t>(cooked.materials.size());
    wpod(buf, matCount);

    for(const auto& kv : cooked.materials){
        const std::string& matName = kv.first;
        const CookedMaterial& m = kv.second;
        wstr(buf, matName);

        uint32_t mt = static_cast<uint32_t>(m.type);
        wpod(buf, mt);

        uint32_t texCount = static_cast<uint32_t>(m.textures.size());
        wpod(buf, texCount);

        for(const auto& tv : m.textures){
            const std::string& slotKey = tv.first;
            const CookedTexture& t = tv.second;
            wstr(buf, slotKey);

            uint32_t usage = static_cast<uint32_t>(t.usage);
            wpod(buf, usage);

            uint16_t flags = t.flags;
            uint16_t pad=0;
            wpod(buf, flags);
            wpod(buf, pad);
            wstr(buf, t.uri);
        }
    }

    return buf;
}

auto Asset::serialize(const CookedMesh& cooked, const std::filesystem::path& outputPath) -> void{
    auto buf = serializeToBuffer(cooked);
    std::ofstream ofs(outputPath, std::ios::binary);
    if(!ofs)
        throw std::runtime_error(
            std::format("failed to open for write: {}",
                outputPath.string()));
    ofs.write(reinterpret_cast<const char*>(buf.data()),
        static_cast<std::streamsize>(buf.size()));
}

auto Asset::loadFromBuffer(const std::vector<uint8_t>& bin) -> CookedMesh{
    R r{ bin.data(), bin.data()+bin.size() };
    FileHeaderV2 FH{}; if(!r.rpod(FH))
        throw std::runtime_error("mbmesh: truncated header");
    if(std::memcmp(FH.magic, "MBMESH\2", 8) != 0 || FH.version != 0)
        throw std::runtime_error("mbmesh: bad magic/version");

    CookedMesh out{};
    if(!r.rpod(out.axisInfo))
        throw std::runtime_error("mbmesh: truncated axis");
    if(!r.rpod(out.aabb))
        throw std::runtime_error("mbmesh: truncated aabb");

    uint32_t smCount=0; if(!r.rpod(smCount))
        throw std::runtime_error("mbmesh: truncated submesh count");
    out.submeshes.resize(smCount);

    for(uint32_t i=0;i<smCount;++i){
        uint32_t prim=0; if(!r.rpod(prim))
            throw std::runtime_error("mbmesh: submesh prim");
        out.submeshes[i].primitiveType = static_cast<PrimitiveType>(prim);
        if(!r.rstr(out.submeshes[i].materialSlotName))
            throw std::runtime_error("mbmesh: submesh slot");
        
        uint32_t vc=0, ic=0;
        if(!r.rpod(vc))
            throw std::runtime_error("mbmesh: submesh vc");

        out.submeshes[i].vertices.resize(vc);
        if(vc && !r.rbytes(out.submeshes[i].vertices.data(), vc*sizeof(Vertex)))
            throw std::runtime_error("mbmesh: submesh verts");
        if(!r.rpod(ic))
            throw std::runtime_error("mbmesh: submesh ic");

        out.submeshes[i].indices.resize(ic);
        if(ic && !r.rbytes(out.submeshes[i].indices.data(), ic*sizeof(uint32_t)))
            throw std::runtime_error("mbmesh: submesh idx");
    }

    uint32_t matCount=0; if(!r.rpod(matCount))
        throw std::runtime_error("mbmesh: mat count");
    for(uint32_t mi=0; mi<matCount; ++mi){
        std::string matName;
        if(!r.rstr(matName))
            throw std::runtime_error("mbmesh: mat name");

        CookedMaterial m{};
        uint32_t mt=0; if(!r.rpod(mt))
            throw std::runtime_error("mbmesh: mat type");

        m.type = static_cast<MaterialType>(mt);
        uint32_t texCount=0;
        if(!r.rpod(texCount))
            throw std::runtime_error("mbmesh: tex count");

        for(uint32_t ti=0; ti<texCount; ++ti){
            std::string slotKey;
            if(!r.rstr(slotKey))
                throw std::runtime_error("mbmesh: slot key");

            CookedTexture t{};

            uint32_t usage=0;
            if(!r.rpod(usage))
                throw std::runtime_error("mbmesh: usage");
            t.usage = static_cast<TextureUsage>(usage);

            uint16_t flags=0,pad=0;
            if(!r.rpod(flags) || !r.rpod(pad))
                throw std::runtime_error("mbmesh: flags");
            t.flags = flags;
            if(!r.rstr(t.uri))
                throw std::runtime_error("mbmesh: uri");
            m.textures.emplace(std::move(slotKey), std::move(t));
        }
        out.materials.emplace(std::move(matName), std::move(m));
    }

    // Recompute AABB if empty (for robustness)
    if(out.submeshes.size() && (out.aabb.min.x==0 && out.aabb.max.x==0)){
        out.aabb = computeAABB(out);
    }
    return out;
}

auto Asset::loadMeshFile(const std::filesystem::path& inputPath) -> CookedMesh{
    std::ifstream ifs(inputPath, std::ios::binary);
    if(!ifs)
        throw std::runtime_error(std::format("failed to open: {}", inputPath.string()));

    std::vector<uint8_t> bin((std::istreambuf_iterator<char>(ifs)), {});
    return loadFromBuffer(bin);
}

void Asset::printLoadedMesh(const CookedMesh& cooked){
    std::println("CookedMesh: submeshes={}, materials={}",
            cooked.submeshes.size(), cooked.materials.size());
    std::println("  Axis: hand={}, up={}, fwd={}, flipV={}, unit={}",
        (int)cooked.axisInfo.hand, cooked.axisInfo.up, cooked.axisInfo.forward,
        cooked.axisInfo.flipV?1:0, cooked.axisInfo.unit);
    std::println("  AABB: min=({}, {}, {}), max=({}, {}, {})",
        cooked.aabb.min.x, cooked.aabb.min.y, cooked.aabb.min.z,
        cooked.aabb.max.x, cooked.aabb.max.y, cooked.aabb.max.z);

    for(size_t i=0;i<cooked.submeshes.size();++i){
        const auto& sm = cooked.submeshes[i];
        std::println("  SM[{}]: verts={}, idx={}, prim={}, slot={}",
            i, sm.vertices.size(), sm.indices.size(),
            (int)sm.primitiveType, sm.materialSlotName);
    }

    for(const auto& [name, m] : cooked.materials){
        std::println("  MAT[{}]: type={}, texCount={}",
            name, (int)m.type, m.textures.size());
        for(const auto& [slot, t] : m.textures){
            std::println("    TEX[{}]: usage={}, flags={}, uri={}",
                slot, (int)t.usage, (unsigned)t.flags, t.uri);
        }
    }
}
