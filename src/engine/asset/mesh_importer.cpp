#include <fstream>
#include <functional>
#include <memory>
#include <print>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "engine/asset/mesh_importer.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Asset;


namespace fs = std::filesystem;

// Alignment helpers
static constexpr uint32_t kFileAlign = 16;
static inline uint32_t alignUp(uint32_t v, uint32_t a = kFileAlign){
    return (v + (a - 1)) & ~(a - 1);
}
static inline void writeZeroPadding(std::ostream& os, uint32_t bytes){
    if(bytes == 0) return;
    static const char zeros[16] = {0};
    while(bytes){
        uint32_t chunk = bytes > 16 ? 16 : bytes;
        os.write(zeros, chunk);
        bytes -= chunk;
    }
}

struct RawScene{
    std::unique_ptr<aiScene, void(*)(aiScene*)> scene{
        nullptr, [](aiScene* s){
            if(s) aiReleaseImport(s);
        }
    };
    std::filesystem::path baseDir;
};

static auto import(const fs::path& inputPath)->RawScene;
static auto extractAxisInfo(const RawScene&)->AxisInfo;
static auto buildTransform(
    const AxisInfo& src, const AxisInfo& dst)->Mat4;
static auto buildMesh(const RawScene&, const AxisInfo& dst)->CookedMesh;

static TextureUsage mapTextureUsage(aiTextureType t);
static bool decodeEmbeddedTextureRGBA8(const aiTexture* tex,
    int& w, int& h, std::vector<uint8_t>& outPixels);
static bool loadTextureRGBA8FromPath(const fs::path& path,
    int& w, int& h, std::vector<uint8_t>& outPixels);
static uint32_t appendTexture(CookedMesh& out,
    TextureUsage usage,
    const aiScene* scene, const aiMaterial* mat,
    aiTextureType type, unsigned texIndex,
    const fs::path& baseDir);

auto Asset::importModelFile(const fs::path& inputPath,
    const CookOptions& options
)->CookedMesh{
    auto rawScene = import(inputPath);

    return buildMesh(rawScene, options.axes);
}

static auto extractHeader(const CookedMesh&)->Header;

void Asset::serialize(const CookedMesh& cooked, const fs::path& outputPath){
    std::ofstream ofs(outputPath, std::ios::binary);
    if(!ofs)
        throw std::runtime_error("failed to open output file");

    Header header = extractHeader(cooked);

    ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
    ofs.write(reinterpret_cast<const char*>(&cooked.axisInfo), sizeof(cooked.axisInfo));
    ofs.write(reinterpret_cast<const char*>(&cooked.aabb), sizeof(cooked.aabb));

    // Helper to compute current offset
    auto tellu32 = [&](){ return static_cast<uint32_t>(ofs.tellp()); };

    // Submesh table (aligned)
    {
        uint32_t cur = tellu32();
        if(cur < header.submeshTableByteOffset)
            writeZeroPadding(ofs, header.submeshTableByteOffset - cur);
        if(!cooked.submeshInfoTable.empty())
            ofs.write(reinterpret_cast<const char*>(cooked.submeshInfoTable.data()),
                      cooked.submeshInfoTable.size()*sizeof(SubmeshInfo));
    }

    // Material table (aligned)
    {
        uint32_t cur = tellu32();
        if(cur < header.materialTableByteOffset)
            writeZeroPadding(ofs, header.materialTableByteOffset - cur);
        if(!cooked.materialInfoTable.empty())
            ofs.write(reinterpret_cast<const char*>(cooked.materialInfoTable.data()),
                      cooked.materialInfoTable.size()*sizeof(MaterialInfo));
    }

    // Texture info table (aligned)
    {
        uint32_t cur = tellu32();
        if(cur < header.textureInfoTableByteOffset)
            writeZeroPadding(ofs, header.textureInfoTableByteOffset - cur);
        if(!cooked.textureInfoTable.empty())
            ofs.write(reinterpret_cast<const char*>(cooked.textureInfoTable.data()),
                      cooked.textureInfoTable.size()*sizeof(TextureInfo));
    }

    // Vertices (aligned)
    {
        uint32_t cur = tellu32();
        if(cur < header.verticesSectionByteOffset)
            writeZeroPadding(ofs, header.verticesSectionByteOffset - cur);
        if(!cooked.vertices.empty())
            ofs.write(reinterpret_cast<const char*>(cooked.vertices.data()),
                      cooked.vertices.size()*sizeof(Vertex));
    }

    // Indices (aligned)
    {
        uint32_t cur = tellu32();
        if(cur < header.indicesSectionByteOffset)
            writeZeroPadding(ofs, header.indicesSectionByteOffset - cur);
        if(!cooked.indices.empty())
            ofs.write(reinterpret_cast<const char*>(cooked.indices.data()),
                      cooked.indices.size()*sizeof(uint32_t));
    }

    // Pixels (aligned)
    {
        uint32_t cur = tellu32();
        if(cur < header.pixelsSectionByteOffset)
            writeZeroPadding(ofs, header.pixelsSectionByteOffset - cur);
        if(!cooked.pixels.empty())
            ofs.write(reinterpret_cast<const char*>(cooked.pixels.data()),
                      cooked.pixels.size()*sizeof(uint8_t));
    }
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
    rawScene.baseDir = inputPath.parent_path();
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

static AABB computeAABB(const CookedMesh& mesh){
    AABB aabb{};
    if(mesh.vertices.empty()) return aabb;
    aabb.min = mesh.vertices[0].position;
    aabb.max = mesh.vertices[0].position;
    for(const auto& v: mesh.vertices){
        aabb.min.x = std::min(aabb.min.x, v.position.x);
        aabb.min.y = std::min(aabb.min.y, v.position.y);
        aabb.min.z = std::min(aabb.min.z, v.position.z);

        aabb.max.x = std::max(aabb.max.x, v.position.x);
        aabb.max.y = std::max(aabb.max.y, v.position.y);
        aabb.max.z = std::max(aabb.max.z, v.position.z);
    }

    return aabb;
}

static auto buildMesh(const RawScene& scene,
    const AxisInfo& dstAxisInfo
)->CookedMesh{
    auto aiScene = scene.scene.get();
    CookedMesh out{};
    if(!scene.scene || !scene.scene->mRootNode)
        return out;

    auto srcAxisInfo = extractAxisInfo(scene);
    Mat4 mat = buildTransform(srcAxisInfo, dstAxisInfo);

    const aiMatrix4x4 aiMat = toAi(mat);

    if(aiScene->HasMaterials()){
        out.materialInfoTable.resize(aiScene->mNumMaterials);
    }

    if(aiScene->HasMaterials()){
        for(unsigned i=0; i<aiScene->mNumMaterials; ++i){
            const aiMaterial* mat = aiScene->mMaterials[i];
            uint32_t start = static_cast<uint32_t>(out.textureInfoTable.size());
            uint32_t count = 0;

            auto tryAppend = [&](aiTextureType t){
                unsigned texCount = mat->GetTextureCount(t);
                for(unsigned ti=0; ti<texCount; ++ti){
                    uint32_t idx = appendTexture(out, mapTextureUsage(t), aiScene, mat, t, ti, scene.baseDir);
                    if(idx != UINT32_MAX) ++count;
                }
            };

            // BaseColor / Diffuse
            tryAppend(aiTextureType_BASE_COLOR);
            tryAppend(aiTextureType_DIFFUSE);
            // Normal
            tryAppend(aiTextureType_NORMALS);
            tryAppend(aiTextureType_NORMAL_CAMERA);
            // Metallic / Roughness (pack as MR if provided separately)
            tryAppend(aiTextureType_METALNESS);
            tryAppend(aiTextureType_DIFFUSE_ROUGHNESS);
            tryAppend(aiTextureType_UNKNOWN); // some exporters put ORM here
            // Emissive
            tryAppend(aiTextureType_EMISSIVE);

            MaterialInfo mi{};
            mi.entrySize = sizeof(MaterialInfo);
            mi.type = count ? MaterialType::PBR : MaterialType::Unlit;
            mi.textureInfoTableIndex = start;
            mi.textureCount = count;
            out.materialInfoTable[i] = mi;
        }
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
                        vx.normal[i] = n[i];
                }
                if(m->HasTangentsAndBitangents()){
                    aiVector3D t = N3 * m->mTangents[v]; t.Normalize();
                    for(int i=0; i<3; ++i)
                        vx.tangent[i] = t[i];
                }
                if(m->HasTextureCoords(0)){
                    vx.texcoord.x = m->mTextureCoords[0][v].x;
                    vx.texcoord.y = dstAxisInfo.flipV ?
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

            SubmeshInfo smi{
                .entrySize = sizeof(SubmeshInfo),
                .verticesSectionIndex = baseVertex,
                .vertexCount = static_cast<uint32_t>(
                    out.vertices.size() - baseVertex),
                .indicesSectionIndex = baseIndex,
                .indexCount = static_cast<uint32_t>(
                    out.indices.size() - baseIndex),
                .materialTableIndex = m->mMaterialIndex
            };
            out.submeshInfoTable.push_back(smi);
        }

        for(unsigned c=0; c<node->mNumChildren; ++c){
            visit(node->mChildren[c], global);
        }
    };

    visit(aiScene->mRootNode, aiMatrix4x4());

    out.header = extractHeader(out);
    out.axisInfo = dstAxisInfo;
    out.aabb = computeAABB(out);

    return out;
}

static TextureUsage mapTextureUsage(aiTextureType t){
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

static bool decodeEmbeddedTextureRGBA8(const aiTexture* tex, int& w, int& h, std::vector<uint8_t>& outPixels){
    if(!tex) return false;
    if(tex->mHeight == 0){
        // Compressed (e.g., PNG/JPG) in memory
        const unsigned char* data = reinterpret_cast<const unsigned char*>(tex->pcData);
        int n = 0;
        unsigned char* img = stbi_load_from_memory(data, tex->mWidth, &w, &h, &n, 4);
        if(!img) return false;
        outPixels.assign(img, img + (w*h*4));
        stbi_image_free(img);
        return true;
    }else{
        // Uncompressed ARGB8888 in pcData
        w = static_cast<int>(tex->mWidth);
        h = static_cast<int>(tex->mHeight);
        outPixels.resize(w*h*4);
        for(int i=0;i<w*h;++i){
            const aiTexel& t = tex->pcData[i];
            outPixels[i*4+0] = t.r;
            outPixels[i*4+1] = t.g;
            outPixels[i*4+2] = t.b;
            outPixels[i*4+3] = t.a;
        }
        return true;
    }
}

static bool loadTextureRGBA8FromPath(const fs::path& path, int& w, int& h, std::vector<uint8_t>& outPixels){
    int n = 0;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* img = stbi_load(path.string().c_str(), &w, &h, &n, 4);
    if(!img) return false;
    outPixels.assign(img, img + (w*h*4));
    stbi_image_free(img);
    return true;
}

static uint32_t appendTexture(CookedMesh& out, TextureUsage usage,
                              const aiScene* scene, const aiMaterial* mat,
                              aiTextureType type, unsigned texIndex,
                              const fs::path& baseDir){
    aiString texPath; aiTextureMapMode mapU, mapV;
    if(mat->GetTexture(type, texIndex, &texPath, nullptr, nullptr, nullptr, nullptr, &mapU) != aiReturn_SUCCESS)
        return UINT32_MAX;

    std::vector<uint8_t> rgba;
    int w=0,h=0;

    if(texPath.length > 0 && texPath.C_Str()[0] == '*'){
        // Embedded texture: path like "*0"
        int embeddedIndex = std::atoi(texPath.C_Str()+1);
        if(embeddedIndex >= 0 && embeddedIndex < static_cast<int>(scene->mNumTextures)){
            const aiTexture* tex = scene->mTextures[embeddedIndex];
            if(!decodeEmbeddedTextureRGBA8(tex, w, h, rgba)) return UINT32_MAX;
        }else{
            return UINT32_MAX;
        }
    }else{
        fs::path full = baseDir / fs::path(texPath.C_Str());
        if(!loadTextureRGBA8FromPath(full, w, h, rgba)){
            throw std::runtime_error(std::string{"Failed to load external texture: "} + full.string());
        }
    }

    uint32_t pixelIndex = static_cast<uint32_t>(out.pixels.size());
    out.pixels.insert(out.pixels.end(), rgba.begin(), rgba.end());

    TextureInfo ti{};
    ti.entrySize = sizeof(TextureInfo);
    ti.usage = usage;
    ti.format = TextureFormat::R8G8B8A8_SRGB; // treat color as sRGB by default
    ti.width = static_cast<uint16_t>(w);
    ti.height = static_cast<uint16_t>(h);
    ti.pixelSectionIndex = pixelIndex;
    ti.pixelCount = static_cast<uint32_t>(rgba.size());

    uint32_t texInfoIndex = static_cast<uint32_t>(out.textureInfoTable.size());
    out.textureInfoTable.push_back(ti);
    return texInfoIndex;
}

static auto extractHeader(const CookedMesh& cooked)->Header{
    Header header;

    const char magic[]="MBMESH\1";
    std::memcpy(header.magic, magic, 8);
    header.version = 1;
    header.headerSize = sizeof(Header);

    // Start after fixed header + axisInfo + aabb
    uint32_t cur = sizeof(Header) + sizeof(AxisInfo) + sizeof(AABB);

    // Submesh info table
    header.submeshTableStride = sizeof(SubmeshInfo);
    cur = alignUp(cur);
    header.submeshTableByteOffset = cur;
    header.numSubmesh = static_cast<uint32_t>(cooked.submeshInfoTable.size());
    header.submeshTableByteSize = header.numSubmesh * sizeof(SubmeshInfo);
    cur += header.submeshTableByteSize;

    // Material info table
    header.materialTableStride = sizeof(MaterialInfo);
    cur = alignUp(cur);
    header.materialTableByteOffset = cur;
    header.numMaterial = static_cast<uint32_t>(cooked.materialInfoTable.size());
    header.materialTableByteSize = header.numMaterial * sizeof(MaterialInfo);
    cur += header.materialTableByteSize;

    // Texture info table
    header.textureInfoTableStride = sizeof(TextureInfo);
    cur = alignUp(cur);
    header.textureInfoTableByteOffset = cur;
    header.numTexture = static_cast<uint32_t>(cooked.textureInfoTable.size());
    header.textureInfoTableByteSize = header.numTexture * sizeof(TextureInfo);
    cur += header.textureInfoTableByteSize;

    // Vertices section
    header.verticesSectionStride = sizeof(Vertex);
    cur = alignUp(cur);
    header.verticesSectionByteOffset = cur;
    header.numVertex = static_cast<uint32_t>(cooked.vertices.size());
    header.verticesSectionByteSize = header.numVertex * sizeof(Vertex);
    cur += header.verticesSectionByteSize;

    // Indices section
    header.indicesSectionStride = sizeof(uint32_t);
    cur = alignUp(cur);
    header.indicesSectionByteOffset = cur;
    header.numIndex = static_cast<uint32_t>(cooked.indices.size());
    header.indicesSectionByteSize = header.numIndex * sizeof(uint32_t);
    cur += header.indicesSectionByteSize;

    // Pixels blob
    cur = alignUp(cur);
    header.pixelsSectionByteOffset = cur;
    header.pixelsSectionByteSize = static_cast<uint32_t>(cooked.pixels.size() * sizeof(uint8_t));
    cur += header.pixelsSectionByteSize;

    return header;
}
