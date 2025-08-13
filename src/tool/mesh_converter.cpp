#include <filesystem>
#include <print>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <functional>
#include <vector>
#include <cstdint>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <cfloat>
#include <assimp/cimport.h>
#include <cstring>

namespace fs = std::filesystem;

// Inserted struct definitions for SourceMeta and Mat4
struct SourceMeta {
    char up = 'Y';
    char forward = 'Z';
    float unit = 1.0f;
    bool isRightHanded = true;
};

struct Mat4 {
    float m[16];
    Mat4() {
        for(int i=0; i<16; ++i)
            m[i] = (i%5==0) ? 1.0f : 0.0f;
    }
};

struct RawScene{
    std::unique_ptr<aiScene, void(*)(aiScene*)> scene{nullptr, [](aiScene* s){ if(s) aiReleaseImport(s); }};
};

struct Vertex; // forward declaration so CookedMesh can hold std::vector<Vertex>

struct CookedMesh{
    struct Header {
        char     magic[8] { 'M','B','M','E','S','H','\x01','\0' };
        uint32_t version = 1;
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
        uint32_t submeshCount = 0;
        uint32_t materialCount = 0;
        uint32_t vertexStride = 0; // bytes per-vertex (set on serialize)
        uint32_t indexStride  = 0; // bytes per-index  (set on serialize)
    } header;

    struct Submesh {
        uint32_t indexOffset = 0;
        uint32_t indexCount = 0;
        uint32_t materialIndex = 0;
    };

    std::vector<Submesh> submeshes;
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;
    std::vector<int> materials;
    float aabbMin[3] {  FLT_MAX,  FLT_MAX,  FLT_MAX };
    float aabbMax[3] { -FLT_MAX, -FLT_MAX, -FLT_MAX };
};

struct TargetAxes {
    enum Hand { RH, LH } hand = LH;
    char up = 'Y';
    char forward = 'Z';
    float unit = 0.01f;
    bool flipV = true;
};

struct CookOptions {
    TargetAxes axes;
    bool keepSkin = true;
    bool genTangents = true;
};

struct Vertex {
    // Layout MUST match shader expectation: position -> normal -> uv -> tangent
    float px, py, pz;        // position (float3)
    float nx, ny, nz;        // normal   (float3)
    float u0, v0;            // uv0      (float2)
    float tx, ty, tz;        // tangent  (float3), no handedness w
};

static aiMatrix4x4 ToAi(const Mat4& M);
static float Det3x3(const aiMatrix3x3& A);
static Mat4 BuildTransform(const TargetAxes& dst, const SourceMeta& src);
static void ApplyTransform(aiScene* scene, const Mat4& M, bool flipV);
static CookedMesh BuildMesh(const aiScene* scene, const Mat4& M, bool flipV);
static void ComputeAABB(CookedMesh& out);
static void SerializeMesh(const CookedMesh& m, const fs::path& outPath);

auto Import(const fs::path& input_path)->RawScene{
    Assimp::Importer importer;
    const unsigned flags =
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_ImproveCacheLocality |
        aiProcess_SortByPType |
        aiProcess_ValidateDataStructure;

    const aiScene* loaded = importer.ReadFile(input_path.string(), flags);
    if(!loaded){
        throw std::runtime_error(std::string{"Assimp failed: "} + importer.GetErrorString());
    }
    aiScene* orphaned = importer.GetOrphanedScene();
    RawScene rs; rs.scene.reset(orphaned); return rs;
}

static aiMatrix4x4 ToAi(const Mat4& M){
    return aiMatrix4x4(
        M.m[0], M.m[4], M.m[8],  M.m[12],
        M.m[1], M.m[5], M.m[9],  M.m[13],
        M.m[2], M.m[6], M.m[10], M.m[14],
        M.m[3], M.m[7], M.m[11], M.m[15]
    );
}

static float Det3x3(const aiMatrix3x3& A){
    return A.a1*(A.b2*A.c3 - A.b3*A.c2)
         - A.a2*(A.b1*A.c3 - A.b3*A.c1)
         + A.a3*(A.b1*A.c2 - A.b2*A.c1);
}

static Mat4 BuildTransform(const TargetAxes& dst, const SourceMeta& src){
    Mat4 M; // identity
    // Unit scaling: bring source to destination units. Interpret `unit` as meters-per-source-unit.
    // Example: src.unit=0.01 (cm), dst.unit=1.0 (m)  => scale = src.unit / dst.unit? We want values expressed in dst units:
    // position_dst = position_src * (src_unit_in_meters / dst_unit_in_meters)
    const float scale = (src.unit <= 0.f ? 1.f : src.unit) / (dst.unit <= 0.f ? 1.f : dst.unit);
    M.m[0] *= scale; M.m[5] *= scale; M.m[10] *= scale;

    // Handedness: if src is RH and dst requests LH, flip X
    if(src.isRightHanded && dst.hand == TargetAxes::LH){
        M.m[0] *= -1.0f;
    }
    // NOTE: up/forward assumed Y-up/Z-forward for now. Extend with axis-rotation if needed.
    return M;
}

static CookedMesh BuildMesh(const aiScene* scene, const Mat4& M, bool flipV){
    CookedMesh out{};
    if(!scene || !scene->mRootNode) return out;

    const aiMatrix4x4 M_ai = ToAi(M);

    // Gather materials placeholder (optional: map aiMaterial to MaterialDesc later)
    if(scene->HasMaterials()){
        out.materials.resize(scene->mNumMaterials);
    }

    // Recursive traversal
    std::function<void(const aiNode*, aiMatrix4x4)> Visit;
    Visit = [&](const aiNode* node, aiMatrix4x4 parent){
        aiMatrix4x4 global = parent * node->mTransformation; // node local applied after parent
        aiMatrix4x4 T = M_ai * global;                        // dst = M ∘ global ∘ v

        // 3x3 normal matrix and its determinant sign (for winding)
        aiMatrix3x3 T3(T); // upper-left 3x3
        aiMatrix3x3 N3 = T3; N3.Inverse(); N3.Transpose();
        const float detT = Det3x3(T3);
        const bool flipWinding = (detT < 0.0f);

        for(unsigned im=0; im<node->mNumMeshes; ++im){
            const aiMesh* m = scene->mMeshes[node->mMeshes[im]];
            const uint32_t baseVertex = (uint32_t)out.vertices.size();
            const uint32_t baseIndex  = (uint32_t)out.indices.size();

            // vertices
            for(unsigned v=0; v<m->mNumVertices; ++v){
                Vertex vx{};
                if(m->HasPositions()){
                    aiVector3D p = T * m->mVertices[v];
                    vx.px=p.x; vx.py=p.y; vx.pz=p.z;
                }
                if(m->HasNormals()){
                    aiVector3D n = N3 * m->mNormals[v]; n.Normalize();
                    vx.nx=n.x; vx.ny=n.y; vx.nz=n.z;
                }
                if(m->HasTangentsAndBitangents()){
                    aiVector3D t = N3 * m->mTangents[v]; t.Normalize();
                    vx.tx=t.x; vx.ty=t.y; vx.tz=t.z;
                }
                if(m->HasTextureCoords(0)){
                    vx.u0 = m->mTextureCoords[0][v].x;
                    vx.v0 = flipV ? (1.0f - m->mTextureCoords[0][v].y) : m->mTextureCoords[0][v].y;
                }
                out.vertices.push_back(vx);
            }

            // indices (triangulated)
            CookedMesh::Submesh sm{}; sm.indexOffset = baseIndex; sm.materialIndex = m->mMaterialIndex;
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
            sm.indexCount = (uint32_t)out.indices.size() - sm.indexOffset;
            out.submeshes.push_back(sm);
        }

        for(unsigned c=0; c<node->mNumChildren; ++c){
            Visit(node->mChildren[c], global);
        }
    };

    Visit(scene->mRootNode, aiMatrix4x4()); // parent = identity

    // Fill header
    out.header.vertexCount  = (uint32_t)out.vertices.size();
    out.header.indexCount   = (uint32_t)out.indices.size();
    out.header.submeshCount = (uint32_t)out.submeshes.size();
    out.header.materialCount= (uint32_t)out.materials.size();
    return out;
}

static void ApplyTransform(aiScene* /*scene*/, const Mat4& /*M*/, bool /*flipV*/){
    // No-op: we bake node and axis transforms during BuildMesh for determinism.
}

static void ComputeAABB(CookedMesh& m){
    for(const auto& v : m.vertices){
        m.aabbMin[0] = std::min(m.aabbMin[0], v.px);
        m.aabbMin[1] = std::min(m.aabbMin[1], v.py);
        m.aabbMin[2] = std::min(m.aabbMin[2], v.pz);
        m.aabbMax[0] = std::max(m.aabbMax[0], v.px);
        m.aabbMax[1] = std::max(m.aabbMax[1], v.py);
        m.aabbMax[2] = std::max(m.aabbMax[2], v.pz);
    }
}

static void SerializeMesh(const CookedMesh& m, const fs::path& outPath){
    std::ofstream ofs(outPath, std::ios::binary);
    if(!ofs) throw std::runtime_error("failed to open output file");

    CookedMesh::Header hdr = m.header; // start from defaults
    // Ensure magic/version are set deterministically
    const char kMagic[8] = { 'M','B','M','E','S','H','\x01','\0' };
    std::memcpy(hdr.magic, kMagic, 8);
    hdr.version       = 1;
    hdr.vertexCount   = static_cast<uint32_t>(m.vertices.size());
    hdr.indexCount    = static_cast<uint32_t>(m.indices.size());
    hdr.submeshCount  = static_cast<uint32_t>(m.submeshes.size());
    hdr.materialCount = static_cast<uint32_t>(m.materials.size());
    hdr.vertexStride  = static_cast<uint32_t>(sizeof(Vertex));
    hdr.indexStride   = static_cast<uint32_t>(sizeof(uint32_t)); // 32-bit indices for now

    ofs.write(reinterpret_cast<const char*>(&hdr), sizeof(hdr));
    if(!m.vertices.empty()) ofs.write(reinterpret_cast<const char*>(m.vertices.data()), m.vertices.size()*sizeof(Vertex));
    if(!m.indices.empty())  ofs.write(reinterpret_cast<const char*>(m.indices.data()),  m.indices.size()*sizeof(uint32_t));
    if(!m.submeshes.empty())ofs.write(reinterpret_cast<const char*>(m.submeshes.data()),m.submeshes.size()*sizeof(CookedMesh::Submesh));
}

auto FbxMain(const fs::path& input_path, const fs::path& output_dir, const CookOptions& options = {})->int{
    try{
        RawScene rs = Import(input_path);
        // TODO: Extract real metadata from rs.scene->mMetaData if available
        SourceMeta meta; meta.up='Y'; meta.forward='Z'; meta.unit=1.0f; meta.isRightHanded=true;

        Mat4 M = BuildTransform(options.axes, meta);
        CookedMesh cooked = BuildMesh(rs.scene.get(), M, options.axes.flipV);
        ComputeAABB(cooked);

        fs::path outPath = output_dir / (input_path.stem().string() + ".mbmesh");
        SerializeMesh(cooked, outPath);
        std::println("[OK] wrote {}", outPath.string());
        return 0;
    }catch(const std::exception& e){
        std::println("[ERR] {}", e.what());
        return 1;
    }
}

auto main(int argc, char* argv[])->int{
    if(argc < 5){
        std::println("usage: [--in] <input path> [--out] <output dir>");
        return 1;
    }

    fs::path input_path;
    fs::path output_dir;

    for(int i=1; i<argc; ++i){
        std::string arg = argv[i];
        if((arg == "--in") && i+1 < argc)
            input_path = argv[++i];
        else if((arg == "--out") && i+1 < argc)
            output_dir = argv[++i];
    }
    if(input_path.empty()){
        std::println("input file not specified");
        return 1;
    }
    if(output_dir.empty()){
        std::println("output directory not specified");
        return 1;
    }

    if(!fs::exists(input_path) || !fs::is_regular_file(input_path)){
        std::println("no such file: {}", input_path.string());
        return 1;
    }
    if(!fs::exists(output_dir) || !fs::is_directory(output_dir)){
        std::println("no such directory: {}", output_dir.string());
        return 1;
    }

    auto ext = input_path.extension().string();
    for(char& c : ext) c = (char)std::tolower((unsigned char)c);
    if(ext == ".fbx" || ext == ".gltf" || ext == ".glb"){
        return FbxMain(input_path, output_dir);
    } else {
        std::println("{} not supported: {}", ext, input_path.string());
        return 1;
    }
}