#include <algorithm>
#include <cmath>
#include <cstring>
#include <format>
#include <fstream>
#include <functional>
#include <memory>
#include <numbers>
#include <print>
#include <string>
#include <sstream>
#include <vector>
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

auto Asset::importMeshFile(const fs::path& inputPath,
    const CookOptions& options
)->CookedMesh{
    auto rawScene = import(inputPath);

    return buildMesh(rawScene, options.axes);
}

static void writeCooked(std::ostream& ofs, const CookedMesh& cooked){
    if(!ofs)
        throw std::runtime_error("failed to open output stream");

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

        if(!cooked.materialInfoTable.empty()){
            std::vector<MaterialInfo> mats = cooked.materialInfoTable;

            uint32_t nameCur = header.materialNameByteOffset;
            const size_t n = mats.size();
            for(size_t i=0;i<n;++i){
                const uint32_t sz = (i < cooked.materialNameTable.size())
                    ? static_cast<uint32_t>(cooked.materialNameTable[i].size())
                    : 0u;
                mats[i].nameByteOffset = (sz>0) ? nameCur : 0u;
                mats[i].nameByteSize   = sz;
                nameCur += sz;
            }
        
            ofs.write(reinterpret_cast<const char*>(mats.data()),
                      mats.size()*sizeof(MaterialInfo));
        }
    }

    // Material names blob (aligned)
    {
        uint32_t cur2 = tellu32();
        if(cur2 < header.materialNameByteOffset)
            writeZeroPadding(ofs, header.materialNameByteOffset - cur2);

        if(header.materialNameByteSize){
            for(const auto& s : cooked.materialNameTable){
                if(!s.empty()){
                    // not null-terminated
                    ofs.write(s.data(), static_cast<std::streamsize>(s.size()));
                }
            }
        }
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

void Asset::serialize(const CookedMesh& cooked, const fs::path& outputPath){
    std::ofstream ofs(outputPath, std::ios::binary);
    writeCooked(ofs, cooked);
}

auto Asset::serializeToBuffer(const CookedMesh& cooked) -> std::vector<uint8_t>{
    std::ostringstream oss(std::ios::binary);
    writeCooked(oss, cooked);
    const std::string blob = oss.str();
    return std::vector<uint8_t>(blob.begin(), blob.end());
}

static CookedMesh readCooked(std::istream& ifs){
    CookedMesh cooked{};

    if(!ifs) throw std::runtime_error("failed to open .mbmesh stream");

    // Read fixed header + axis + aabb
    Header H{}; AxisInfo AX{}; AABB AA{};
    ifs.read(reinterpret_cast<char*>(&H), sizeof(H));
    if(!ifs) throw std::runtime_error("failed to read mbmesh header");

    // Basic magic/version checks
    const char expectedMagic[8] = "MBMESH\1";
    if(std::memcmp(H.magic, expectedMagic, 8) != 0)
        throw std::runtime_error("invalid mbmesh magic");
    if(H.version != 1)
        throw std::runtime_error("unsupported mbmesh version");

    ifs.read(reinterpret_cast<char*>(&AX), sizeof(AX));
    if(!ifs) throw std::runtime_error("failed to read axis info");
    ifs.read(reinterpret_cast<char*>(&AA), sizeof(AA));
    if(!ifs) throw std::runtime_error("failed to read aabb");

    // Validate strides
    if(H.submeshTableStride && H.submeshTableStride != sizeof(SubmeshInfo))
        throw std::runtime_error("Submesh stride mismatch");
    if(H.materialTableStride && H.materialTableStride != sizeof(MaterialInfo))
        throw std::runtime_error("Material stride mismatch");
    if(H.textureInfoTableStride && H.textureInfoTableStride != sizeof(TextureInfo))
        throw std::runtime_error("TextureInfo stride mismatch");
    if(H.verticesSectionStride && H.verticesSectionStride != sizeof(Vertex))
        throw std::runtime_error("Vertex stride mismatch");
    if(H.indicesSectionStride && H.indicesSectionStride != sizeof(uint32_t))
        throw std::runtime_error("Index stride mismatch");

    auto read_at = [&](uint32_t offset, void* dst, uint32_t bytes){
        if(bytes == 0) return;
        ifs.seekg(static_cast<std::streamoff>(offset), std::ios::beg);
        if(!ifs) throw std::runtime_error("seek failed while reading mbmesh");
        ifs.read(reinterpret_cast<char*>(dst), bytes);
        if(!ifs) throw std::runtime_error("read failed while reading mbmesh");
    };

    // Submesh table
    cooked.submeshInfoTable.resize(H.numSubmesh);
    if(H.submeshTableByteSize && H.numSubmesh){
        read_at(H.submeshTableByteOffset,
            cooked.submeshInfoTable.data(),
            H.numSubmesh * sizeof(SubmeshInfo));
    }

    // Material table
    cooked.materialInfoTable.resize(H.numMaterial);
    if(H.materialTableByteSize && H.numMaterial){
        read_at(H.materialTableByteOffset,
            cooked.materialInfoTable.data(),
            H.numMaterial * sizeof(MaterialInfo));
    }

    // Material names blob
    if(H.materialNameByteSize){
        std::vector<char> nameBlob(H.materialNameByteSize);
        read_at(H.materialNameByteOffset, nameBlob.data(), H.materialNameByteSize);

        cooked.materialNameTable.resize(H.numMaterial);
        for(uint32_t i=0;i<H.numMaterial;++i){
            const auto& mi = cooked.materialInfoTable[i];
            if(mi.nameByteSize == 0){
                cooked.materialNameTable[i].clear();
                continue;
            }
            if(mi.nameByteOffset < H.materialNameByteOffset){
                cooked.materialNameTable[i].clear();
                continue;
            }
            uint32_t rel = mi.nameByteOffset - H.materialNameByteOffset;
            if(rel + mi.nameByteSize <= H.materialNameByteSize){
                cooked.materialNameTable[i].assign(
                    nameBlob.data()+rel,
                    nameBlob.data()+rel+mi.nameByteSize
                );
            } else{
                cooked.materialNameTable[i].clear();
            }
        }
    }

    // Texture info table
    cooked.textureInfoTable.resize(H.numTexture);
    if(H.textureInfoTableByteSize && H.numTexture){
        read_at(H.textureInfoTableByteOffset,
            cooked.textureInfoTable.data(),
            H.numTexture * sizeof(TextureInfo));
    }

    // Vertices
    cooked.vertices.resize(H.numVertex);
    if(H.verticesSectionByteSize && H.numVertex){
        read_at(H.verticesSectionByteOffset,
            cooked.vertices.data(),
            H.numVertex * sizeof(Vertex));
    }

    // Indices
    cooked.indices.resize(H.numIndex);
    if(H.indicesSectionByteSize && H.numIndex){
        read_at(H.indicesSectionByteOffset,
            cooked.indices.data(),
            H.numIndex * sizeof(uint32_t));
    }

    // Pixels blob
    cooked.pixels.resize(H.pixelsSectionByteSize);
    if(H.pixelsSectionByteSize){
        read_at(H.pixelsSectionByteOffset,
            cooked.pixels.data(),
            H.pixelsSectionByteSize);
    }

    // Assign header, axis, aabb
    cooked.header = H;
    cooked.axisInfo = AX;
    cooked.aabb = AA;

    return cooked;
}

auto Asset::loadMeshFile(const fs::path& inputPath) -> CookedMesh{
    std::ifstream ifs(inputPath, std::ios::binary);
    return readCooked(ifs);
}

auto Asset::loadFromBuffer(const std::vector<uint8_t>& buf) -> CookedMesh{
    std::istringstream iss(std::string(reinterpret_cast<const char*>(buf.data()), buf.size()), std::ios::binary);
    return readCooked(iss);
}

void Asset::printLoadedMesh(const CookedMesh& cooked){
    using std::println;

    const auto& H = cooked.header;

    // Header block
    println("mbmesh");
    println("├─ header");
    // Magic as ASCII up to 6 chars, then show raw byte 6 as \\x01 style if present
    std::string magicAscii;
    for(int i=0;i<8;++i){
        unsigned char c = (unsigned char)H.magic[i];
        if(c>=32 && c<=126) magicAscii.push_back((char)c);
        else{
            char tmp[8];
            std::snprintf(tmp, sizeof(tmp), "\\x%02X", c);
            magicAscii += tmp;
        }
    }
    println("│  ├─ magic         : {}", magicAscii);
    println("│  ├─ version       : {}", H.version);
    println("│  ├─ vertexCount   : {}", cooked.vertices.size());
    println("│  ├─ indexCount    : {} ({} triangles)", cooked.indices.size(), cooked.indices.size()/3);
    println("│  ├─ submeshCount  : {}", cooked.submeshInfoTable.size());
    println("│  ├─ materialCount : {}", cooked.materialInfoTable.size());
    println("│  └─ vertexStride  : {} bytes", (unsigned)sizeof(Vertex));

    // Layout block (best-effort; relies on Vertex having these fields in this order)
    println("├─ layout");
    println("│  ├─ pos     @{} (float3)", (unsigned)offsetof(Vertex, position));
    println("│  ├─ normal  @{} (float3)", (unsigned)offsetof(Vertex, normal));
    println("│  ├─ uv0     @{} (float2)", (unsigned)offsetof(Vertex, texcoord));
    println("│  ├─ tangent @{} (float3)", (unsigned)offsetof(Vertex, tangent));
    println("│  └─ stride  ={}", (unsigned)sizeof(Vertex));

    // Offsets block
    auto hexOff = [](uint32_t off){ char b[32]; std::snprintf(b,sizeof(b),"0x%08X",off); return std::string(b); };
    auto human = [](uint64_t n){ char b[64]; const char* u[] = {"B","KB","MB","GB"}; double v=n; int k=0; while(v>=1024.0 && k<3){ v/=1024.0; ++k;} std::snprintf(b,sizeof(b),"%.2f %s", v, u[k]); return std::string(b); };
    println("├─ offsets");
    println("│  ├─ submeshes: {} ({})", hexOff(H.submeshTableByteOffset), human(H.submeshTableByteSize));
    println("│  ├─ materials: {} ({})", hexOff(H.materialTableByteOffset), human(H.materialTableByteSize));
    println("│  ├─ textures : {} ({})", hexOff(H.textureInfoTableByteOffset), human(H.textureInfoTableByteSize));
    println("│  ├─ vertices : {} ({})", hexOff(H.verticesSectionByteOffset), human(H.verticesSectionByteSize));
    println("│  ├─ indices  : {} ({})",  hexOff(H.indicesSectionByteOffset),  human(H.indicesSectionByteSize));
    println("│  └─ pixels   : {} ({})",  hexOff(H.pixelsSectionByteOffset),   human(H.pixelsSectionByteSize));

    // Geometry validity checks
    println("├─ geometry");
    uint32_t maxIdx = 0; uint32_t oor = 0; uint32_t degenerates = 0;
    const uint32_t vcount = (uint32_t)cooked.vertices.size();
    for(uint32_t i=0;i<(uint32_t)cooked.indices.size(); ++i){
        uint32_t idx = cooked.indices[i];
        maxIdx = std::max(maxIdx, idx);
        if(idx >= vcount) ++oor;
    }
    for(uint32_t t=0; t+2 < (uint32_t)cooked.indices.size(); t+=3){
        uint32_t i0=cooked.indices[t], i1=cooked.indices[t+1], i2=cooked.indices[t+2];
        if(i0==i1 || i1==i2 || i2==i0){ ++degenerates; continue; }
        const auto& a = cooked.vertices[i0].position;
        const auto& b = cooked.vertices[i1].position;
        const auto& c = cooked.vertices[i2].position;
        Vec3 cr=cross(b-a, c-a);
        float area2 = std::sqrt(cr.x*cr.x + cr.y*cr.y + cr.z*cr.z);
        if(area2 < 1e-6f) ++degenerates;
    }
    println("│  ├─ indexRange       : [0..{}] {}", maxIdx, (maxIdx < 65536? "OK (U16-capable)":"(needs U32)"));
    println("│  ├─ degenerateTris   : {}", degenerates);
    println("│  └─ outOfRangeIndices: {}", oor);

    // Attribute stats
    auto statLen = [](auto getter, const std::vector<Vertex>& verts){
        double sum=0; double minv=1e9, maxv=-1e9; size_t n=0;
        for(const auto& v: verts){
            Vec3 vv = getter(v);
            double L = std::sqrt((double)vv.x*vv.x + (double)vv.y*vv.y + (double)vv.z*vv.z);
            if(!std::isfinite(L)) continue;
            sum += L; minv = std::min(minv, L); maxv = std::max(maxv, L); ++n;
        }
        return std::tuple<double,double,double,size_t>(n?sum/n:0.0, n?minv:0.0, n?maxv:0.0, n);
    };
    auto [navg,nmin,nmax,nn] = statLen([](const Vertex& v){ return v.normal; }, cooked.vertices);
    auto [tavg,tmin,tmax,tn] = statLen([](const Vertex& v){ return asVec3(v.tangent);}, cooked.vertices);

    double maxAbsDot = 0.0;
    for(const auto& v: cooked.vertices){
        const Vec3& n = v.normal; const Vec3 t = asVec3(v.tangent);
        double dot = (double)n.x*t.x + (double)n.y*t.y + (double)n.z*t.z;
        double nL = std::sqrt((double)n.x*n.x + (double)n.y*n.y + (double)n.z*n.z);
        double tL = std::sqrt((double)t.x*t.x + (double)t.y*t.y + (double)t.z*t.z);
        if(nL>0 && tL>0){ maxAbsDot = std::max(maxAbsDot, std::abs(dot/(nL*tL))); }
    }

    println("├─ attributes");
    println("│  ├─ |N| length       : avg={:.3f} min={:.3f} max={:.3f} {}", navg, nmin, nmax, (std::abs(navg-1.0)<=0.01 && nmin>0.95 && nmax<1.05? "(OK)":""));
    println("│  ├─ |T| length       : avg={:.3f} min={:.3f} max={:.3f} {}", tavg, tmin, tmax, (tavg>0?"(OK)":""));
    println("│  └─ dot(N,T)         : max|.|={:.3f} {}", maxAbsDot, (maxAbsDot<=0.05?"(orthogonal)":""));

    // UV stats
    double umin= std::numeric_limits<double>::infinity(), umax=-umin;
    double vmin= std::numeric_limits<double>::infinity(), vmax=-vmin;
    for(const auto& v: cooked.vertices){ umin = std::min(umin,(double)v.texcoord.x); umax = std::max(umax,(double)v.texcoord.x); vmin = std::min(vmin,(double)v.texcoord.y); vmax = std::max(vmax,(double)v.texcoord.y); }
    println("├─ uv0");
    println("│  ├─ U range          : [{:.2f}..{:.2f}]{}", umin, umax, (umin<0.0||umax>1.0?" (tiling/bleed risk)":""));
    println("│  └─ V range          : [{:.2f}..{:.2f}] (flipV={})", vmin, vmax, cooked.axisInfo.flipV?"true":"false");

    // Bounds
    const auto& B = cooked.aabb;
    auto radius = [&](){ double dx=B.max.x-B.min.x, dy=B.max.y-B.min.y, dz=B.max.z-B.min.z; return 0.5*std::sqrt(dx*dx+dy*dy+dz*dz); }();
    auto cx = 0.5*(B.min.x+B.max.x), cy = 0.5*(B.min.y+B.max.y), cz = 0.5*(B.min.z+B.max.z);
    println("├─ bounds");
    println("│  ├─ AABB             : min({:.3f},{:.3f},{:.3f}) max({:.3f},{:.3f},{:.3f})", B.min.x,B.min.y,B.min.z, B.max.x,B.max.y,B.max.z);
    println("│  └─ sphere           : center({:.3f},{:.3f},{:.3f}) radius={:.3f}", cx,cy,cz, radius);

    // Submeshes
    println("├─ submeshes[{}]", cooked.submeshInfoTable.size());
    for(size_t i=0;i<cooked.submeshInfoTable.size(); ++i){
        const auto& s = cooked.submeshInfoTable[i];
        const char* mname = (s.materialTableIndex < cooked.materialNameTable.size())
            ? cooked.materialNameTable[s.materialTableIndex].c_str() : "";
        println("│  ├─ [{}] off={} cnt={} mat={} {} ({} tris)",
            i, s.indicesSectionIndex, s.indexCount, s.materialTableIndex,
            (std::strlen(mname)? std::format("({})", mname): std::string()),
            s.indexCount/3);
    }

    // Axes (we only have target axis in file)
    const auto& AX = cooked.axisInfo;
    auto handStr = (AX.hand==AxisInfo::RH?"RH":"LH");
    println("└─ axes               : {},{},{} unit={} flipV={}", handStr, char(AX.up), char(AX.forward), AX.unit, AX.flipV?"true":"false");
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
        out.materialNameTable.resize(aiScene->mNumMaterials);
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
            aiString aiName = mat->GetName();
            if(aiName.length > 0){
                out.materialNameTable[i] = std::string(aiName.C_Str());
            } else{
                out.materialNameTable[i] = std::format("Material{}", i);
            }
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
            throw std::runtime_error(
                std::format("Failed to load external texture: {}", full.string()));
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

auto Asset::extractHeader(const CookedMesh& cooked)->Header{
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

    // Material name blob section
    cur = alignUp(cur);
    header.materialNameByteOffset = cur;
    uint32_t nameBlobSize = 0;
    for(const auto& s: cooked.materialNameTable)
        nameBlobSize += static_cast<uint32_t>(s.size()); // without null
    header.materialNameByteSize = nameBlobSize;
    cur += header.materialNameByteSize;

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

RawMesh Asset::createTriangle(){
    Vertices vertices = {
        {
            {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {0.5f, 0.0f}, {0.0f, -1.0f, 0.0f}
        },{
            {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
        },{
            {1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
        }
    };
    Indices indices = {1, 0, 2};

    return { RawMeshPart(vertices, indices) };

}
RawMesh Asset::createRectangle(){
    Vertices vertices = {
        {
            {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, 
            {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
        },{
            {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
        },{
            {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}
        },{
            {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}
        }
    };
    Indices indices = {
        0, 2, 1,
        0, 3, 2
    };

    return { RawMeshPart(vertices, indices) };
}
RawMesh Asset::createCube(){
    Vertices vertices = {
        // front
        {
            {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},
            {0.0f, 1.0f}, {},
        },{
            { 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},
            {1.0f, 1.0f}, {}
        },{
            { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},
            {1.0f, 0.0f}, {}
        },{
            {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},
            {0.0f, 0.0f}, {}
        },
        // back
        {
            {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},
            {0.0f, 1.0f}, {}
        },{
            { 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},
            {1.0f, 1.0f}, {}
        },{
            { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f}, {}
        },{
            {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f}, {}
        },
        // left
        {
            {-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f}, {}
        },{
            {-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f},
            {1.0f, 1.0f}, {}
        },{
            {-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f}, {}
        },{
            {-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f}, {}
        },
        // right
        {
            { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f}, {}
        },{
            { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},
            {1.0f, 1.0f}, {}
        },{
            { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f}, {}
        },{
            { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f}, {}
        },
        // bottom
        {
            {-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f},
            {0.0f, 1.0f}, {}
        },{
            { 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f},
            {1.0f, 1.0f}, {}
        },{
            { 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f},
            {1.0f, 0.0f}, {}
        },{
            {-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f},
            {0.0f, 0.0f}, {}
        },
        // top
        {
            {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f}, {}
        },{
            { 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},
            {1.0f, 1.0f}, {}
        },{
            { 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f},
            {1.0f, 0.0f}, {}
        },{
            {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f}, {}
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

    return { RawMeshPart(vertices, indices) };
}
RawMesh Asset::createSphere(float radius,
    int numSlices, int numStacks
){
    Vertices vertices;
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

            vertices.emplace_back(RawVertex{
                {x, y, z}, {x, y, z},
                {u, v}, {}
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

    return { RawMeshPart(vertices, indices) };
}
