#include <algorithm>
#include <cmath>
#include <cstring>
#include <format>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <numbers>
#include <string>
#include <print>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <iterator>
#include "asset/mesh_importer.hpp"
#include "asset/ktx2_encoder.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Asset;

namespace fs = std::filesystem;

namespace {
    // Write raw bytes to a file (utility)
    bool write_bytes(const fs::path& p, const void* data, size_t n){
        std::error_code ec; fs::create_directories(p.parent_path(), ec);
        std::ofstream ofs(p, std::ios::binary); if(!ofs) return false;
        ofs.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(n));
        return ofs.good();
    }

    // Minimal uncompressed 32-bit TGA writer (RGBA8 -> BGRA on disk, origin bottom-left)
    bool write_tga_rgba8(const fs::path& path, int w, int h, const unsigned char* rgba){
        if(w <= 0 || h <= 0 || !rgba)
            return false;
        std::vector<unsigned char> buf;
        buf.reserve(18 + 4*w*h);

        unsigned char hdr[18]{}; // uncompressed true-color
        hdr[2]  = 2;             // image type = uncompressed true-color
        hdr[12] = static_cast<unsigned char>( w        & 0xFF);
        hdr[13] = static_cast<unsigned char>((w >> 8)  & 0xFF);
        hdr[14] = static_cast<unsigned char>( h        & 0xFF);
        hdr[15] = static_cast<unsigned char>((h >> 8)  & 0xFF);
        hdr[16] = 32;                    // 32 bits/pixel
        hdr[17] = 0;                     // origin bottom-left, alpha bits = 0
        buf.insert(buf.end(), hdr, hdr+18);

        // TGA expects BGRA order per pixel
        const auto pixels = w*h;
        buf.resize(buf.size() + 4*pixels);

        unsigned char* out = buf.data() + 18;
        for(int i=0; i<pixels; ++i){
            const unsigned char* s = rgba + i*4;
            out[i*4 + 0] = s[2]; // B
            out[i*4 + 1] = s[1]; // G
            out[i*4 + 2] = s[0]; // R
            out[i*4 + 3] = s[3]; // A
        }
        return write_bytes(path, buf.data(), buf.size());
    }

    // Export an Assimp embedded texture to a temporary ordinary image file.
    // For compressed blobs (mHeight==0), we dump the blob with an extension hint.
    // For uncompressed images (mHeight>0), we write a simple 32-bit TGA.
    bool export_assimp_embedded_to_temp(const aiTexture* tex,
        const fs::path& outPathHint, fs::path& outRealPath
    ){
        if(!tex)
            return false;
        if(tex->mHeight == 0){
            // Compressed blob; pick extension from achFormatHint if present
            std::string ext = (tex->achFormatHint[0]) ?
                std::string(tex->achFormatHint) : std::string("bin");
            fs::path tmp = outPathHint; tmp.replace_extension(ext);
            if(!write_bytes(tmp, tex->pcData, tex->mWidth))
                return false;
            outRealPath = tmp; return true;
        }else {
            // Uncompressed pixel array (aiTexel is RGBA8)
            const auto w = tex->mWidth;
            const auto h = tex->mHeight;
            std::vector<uint8_t> rgba(4*w*h);
            for(size_t y=0; y<h; ++y){
                for(size_t x=0; x<w; ++x){
                    const size_t idx = (y*w + x);
                    const aiTexel& t = tex->pcData[idx];
                    rgba[idx*4 + 0] = t.r;
                    rgba[idx*4 + 1] = t.g;
                    rgba[idx*4 + 2] = t.b;
                    rgba[idx*4 + 3] = t.a;
                }
            }
            fs::path tmp = outPathHint;
            tmp.replace_extension("tga");

            if(!write_tga_rgba8(tmp, w, h, rgba.data()))
                return false;
            outRealPath = tmp; return true;
        }
    }
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
                if(s)
                    aiReleaseImport(s);
            }
        };
        fs::path baseDir;
        fs::path sourceStem;
    };

    TextureUsage mapTextureUsage(aiTextureType t){
        switch(t){
            case aiTextureType_BASE_COLOR:
                [[fallthrough]];
            case aiTextureType_DIFFUSE:
                return TextureUsage::BaseColor;
            case aiTextureType_NORMALS:
                [[fallthrough]];
            case aiTextureType_NORMAL_CAMERA:
                return TextureUsage::Normal;
            case aiTextureType_METALNESS:
                [[fallthrough]];
            case aiTextureType_DIFFUSE_ROUGHNESS:
                [[fallthrough]];
            case aiTextureType_UNKNOWN:
                return TextureUsage::MR;
            case aiTextureType_EMISSIVE:
                return TextureUsage::Emissive;
            default:
                return TextureUsage::BaseColor;
        }
    }

    const char* usageToSlotName(TextureUsage u){
        switch(u){
        case TextureUsage::BaseColor:
            return "BaseColor";
        case TextureUsage::Normal:
            return "Normal";
        case TextureUsage::MR:
            return "MR";
        case TextureUsage::Emissive:
            return "Emissive";
        default:
            return "Unknown";
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
        rawScene.sourceStem = inputPath.stem();
        return rawScene;
    }

    auto extractAxisInfo([[maybe_unused]] const RawScene& scene)->AxisInfo{
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
                if(!inited){
                    aabb.min = aabb.max = v.position;
                    inited = true;
                }
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

    struct BuildCtx {
        const aiScene* aiScene{};
        AxisInfo dstAxis{};
        aiMatrix4x4 aiWorldFromSrc{};
        fs::path textureOutDir = "asset/textures";
        std::string sceneStem;
        CookedMesh* out{};

        // normRelPath -> filename
        const std::unordered_map<std::string, std::string>* finalNameByExternal = nullptr; 
        const std::unordered_map<uint64_t, std::string>*    finalNameByEmbedded = nullptr;
    };

    auto make_ctx(const RawScene& scene, const AxisInfo& dst){
        auto src = extractAxisInfo(scene);
        Mat4 mat = buildTransform(src, dst);

        BuildCtx ctx{};
        ctx.aiScene = scene.scene.get();
        ctx.dstAxis = dst;
        ctx.aiWorldFromSrc = toAi(mat);
        ctx.sceneStem = scene.sourceStem.string();
        return ctx;
    }

    auto make_ktx_params(TextureUsage u){
        Ktx2CookParams kp{};
        kp.kind = (u==TextureUsage::BaseColor) ? TEXTURE_BASECOLOR :
                  (u==TextureUsage::Emissive) ? TEXTURE_EMISSIVE :
                  (u==TextureUsage::Normal)   ? TEXTURE_NORMAL   : TEXTURE_ORM;
        kp.flag         = GENERATE_MIPMAPS | USE_UASTC;
        kp.uastcLevel   = 2;
        kp.etc1sQuality = 128;
        kp.zstdLevel    = 18;
        return kp;
    }

    struct TexResolve {
        bool embedded = false;
        int  index    = -1;       // valid if embedded
        fs::path src;             // external path (or filled when exporting embedded)
        std::string debug;        // "embedded:N" or normalized path string
    };

    TexResolve resolve_uri(const RawScene& scene, const aiString& texPath){
        TexResolve r{};
        if(texPath.length>0 && texPath.C_Str()[0]=='*'){
            r.embedded = true;
            r.index = std::atoi(texPath.C_Str()+1);
            r.debug = std::format("embedded:{}", r.index);
        } else {
            fs::path full = scene.baseDir / fs::path(texPath.C_Str());
            r.src = full.lexically_normal();
            r.debug = r.src.string();
        }
        return r;
    }

    uint64_t fnv1a64(const void* data, size_t n){
        const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
        uint64_t h = 1469598103934665603ull;
        for(size_t i=0; i<n; ++i){
            h ^= p[i];
            h *= 1099511628211ull;
        }
        return h;
    }
    uint64_t fnv1a64_str(std::string_view s){
        return fnv1a64(s.data(), s.size());
    }
    uint64_t content_hash(const aiTexture* tex){
        if(!tex)
            return 0;
        if(tex->mHeight==0)
            return fnv1a64(tex->pcData, tex->mWidth);
        size_t n = size_t(tex->mWidth)*size_t(tex->mHeight)*4;
        return fnv1a64(tex->pcData, n);
    }

    std::string normalizePathString(const fs::path& p){
        std::string s = p.generic_string();
        return s;
    }
    std::string sanitize(const std::string& s){
        std::string r; r.reserve(s.size());
        for(char c: s){
            bool ok = std::isalnum(c) || c=='_' || c=='-' || c=='.';
            r.push_back(ok ? c : '_');
        }
        return r;
    }
    std::string hex8(uint64_t x){
        char b[17];
        std::snprintf(b, sizeof(b), "%08llx", x & 0xffffffffULL);
        return std::string(b);
    }

    std::string get_final_tex_filename(
        const RawScene& scene, const BuildCtx& ctx, const TexResolve& R
    ){
        if(R.embedded){
            const aiTexture* t = (R.index>=0 && R.index<(int)scene.scene->mNumTextures) ?
                scene.scene->mTextures[R.index] : nullptr;
            uint64_t h = content_hash(t);
            if(ctx.finalNameByEmbedded){
                auto it = ctx.finalNameByEmbedded->find(h);
                if(it != ctx.finalNameByEmbedded->end())
                    return it->second;
            }
            return {};
        }else {
            fs::path rel = fs::path(R.src).lexically_relative(scene.baseDir);
            std::string norm = normalizePathString(rel);
            if(ctx.finalNameByExternal){
                auto it = ctx.finalNameByExternal->find(norm);
                if(it != ctx.finalNameByExternal->end())
                    return it->second;
            }
            return {};
        }
    }

    // void cook_one_texture(
    //     const RawScene& scene, const TexResolve& R,
    //     const fs::path& dst, const Ktx2CookParams& kp,
    //     CookedTexture& outCt, const fs::path& tmpDir
    // ){
    //     if(R.embedded){
    //         const aiTexture* tex = (R.index>=0 && R.index < (int)scene.scene->mNumTextures)
    //             ? scene.scene->mTextures[R.index] : nullptr;
    //         if(!tex){
    //             std::println(std::cerr, "[warn] missing embedded texture: {}", R.debug);
    //             return;
    //         }
    //         fs::path hint = tmpDir / std::format("__emb{}_tmp", R.index);
    //         fs::path real;
    //         if(export_assimp_embedded_to_temp(tex, hint, real)){
    //             cook_texture_uri(real, dst, kp, outCt);
    //             std::error_code rec; fs::remove(real, rec);
    //         } else {
    //             std::println(std::cerr, "[warn] failed to export embedded texture: {}", R.debug);
    //         }
    //     } else {
    //         cook_texture_uri(R.src, dst, kp, outCt);
    //     }
    // }

    void append_textures_for_type(
        const aiMaterial* m, aiTextureType t,
        const RawScene& scene, BuildCtx& ctx,
        CookedMaterial& cm
    ){
        TextureUsage usage = mapTextureUsage(t);
        unsigned n = m->GetTextureCount(t);
        for(unsigned ti=0; ti<n; ++ti){
            aiString texPath;
            if(m->GetTexture(t, ti, &texPath) != aiReturn_SUCCESS)
                continue;

            CookedTexture ct{};
            ct.usage = usage;
            ct.flags = (usage==TextureUsage::BaseColor || usage==TextureUsage::Emissive) ?
                TextureFlag_SRGB : 0;

            TexResolve R = resolve_uri(scene, texPath);
            std::string finalFile = get_final_tex_filename(scene, ctx, R);
            if(!finalFile.empty()){
                ct.uri = (ctx.textureOutDir / finalFile).lexically_normal().string();
            }else {
                ct.uri = R.debug; // 배치 표에서 못 찾았을 때 디버그 표기
            }

            const char* key = usageToSlotName(usage);
            if(!cm.textures.contains(key)) cm.textures.emplace(key, std::move(ct));
        }
    }

    void build_materials(const RawScene& scene, BuildCtx& ctx){
        if(!ctx.aiScene || !ctx.aiScene->HasMaterials())
            return;

        for(unsigned i=0; i<ctx.aiScene->mNumMaterials; ++i){
            const aiMaterial* m = ctx.aiScene->mMaterials[i];

            CookedMaterial cm{};
            aiString aiName = m->GetName();
            cm.name = (aiName.length>0) ?
                std::string(aiName.C_Str()) :
                std::format("Material{}", i);
            cm.type = MaterialType::Unlit;

            append_textures_for_type(m, aiTextureType_BASE_COLOR,        scene, ctx, cm);
            append_textures_for_type(m, aiTextureType_DIFFUSE,           scene, ctx, cm);
            append_textures_for_type(m, aiTextureType_NORMALS,           scene, ctx, cm);
            append_textures_for_type(m, aiTextureType_NORMAL_CAMERA,     scene, ctx, cm);
            append_textures_for_type(m, aiTextureType_METALNESS,         scene, ctx, cm);
            append_textures_for_type(m, aiTextureType_DIFFUSE_ROUGHNESS, scene, ctx, cm);
            append_textures_for_type(m, aiTextureType_UNKNOWN,           scene, ctx, cm);
            append_textures_for_type(m, aiTextureType_EMISSIVE,          scene, ctx, cm);

            if(!cm.textures.empty()) cm.type = MaterialType::PBR;
            ctx.out->materials.emplace(cm.name, std::move(cm));
        }
    }

    void fill_vertices(
        const aiMesh* m, const aiMatrix4x4& T,
        const aiMatrix3x3& N, const AxisInfo& dstAxis,
        CookedSubmesh& sm
    ){
        sm.vertices.reserve(m->mNumVertices);
        for(unsigned v=0; v<m->mNumVertices; ++v){
            Vertex vx{};
            if(m->HasPositions()){
                aiVector3D p = T * m->mVertices[v];
                vx.position = {{p.x,p.y,p.z}};
            }
            if(m->HasNormals()){
                aiVector3D n = N * m->mNormals[v]; n.Normalize();
                vx.normal = {{n.x,n.y,n.z}};
            }
            if(m->HasTangentsAndBitangents()){
                aiVector3D t = N * m->mTangents[v]; t.Normalize();
                vx.tangent = {{t.x,t.y,t.z, -1.0f}};
            }
            if(m->HasTextureCoords(0)){
                vx.texcoord.x = m->mTextureCoords[0][v].x;
                vx.texcoord.y = dstAxis.flipV ?
                    (1.0f - m->mTextureCoords[0][v].y) :
                    m->mTextureCoords[0][v].y;
            }
            sm.vertices.push_back(vx);
        }
    }

    void fill_indices(const aiMesh* m, bool flipW, CookedSubmesh& sm){
        sm.indices.reserve(m->mNumFaces*3);
        for(unsigned f=0; f<m->mNumFaces; ++f){
            const aiFace& face = m->mFaces[f];
            if(face.mNumIndices==3){
                uint32_t i0=face.mIndices[0], i1=face.mIndices[1], i2=face.mIndices[2];
                if(flipW)
                    sm.indices.insert(sm.indices.end(), {i0,i2,i1});
                else
                    sm.indices.insert(sm.indices.end(), {i0,i1,i2});
            }
        }
    }

    void visit_node(const aiNode* node, const aiMatrix4x4& parent, BuildCtx& ctx){
        aiMatrix4x4 global = parent * node->mTransformation;
        aiMatrix4x4 T = ctx.aiWorldFromSrc * global;
        aiMatrix3x3 T3(T);
        aiMatrix3x3 N3 = T3; N3.Inverse();
        N3.Transpose();
        bool flipW = (Det3x3(T3) < 0.0f);

        for(unsigned im=0; im<node->mNumMeshes; ++im){
            const aiMesh* m = ctx.aiScene->mMeshes[node->mMeshes[im]];
            CookedSubmesh sm{};
            sm.primitiveType = PrimitiveType::TriangleList;
            sm.materialSlotName = node->mName.length>0 ?
                std::format("{}#{}", node->mName.C_Str(), im) :
                std::format("Submesh#{}", ctx.out->submeshes.size());
            fill_vertices(m, T, N3, ctx.dstAxis, sm);
            fill_indices(m, flipW, sm);
            ctx.out->submeshes.push_back(std::move(sm));
        }

        for(unsigned c=0; c<node->mNumChildren; ++c)
            visit_node(node->mChildren[c], global, ctx);
    }

    auto buildMesh(
            const RawScene& scene,
            BuildCtx& ctx
        )->CookedMesh{
        CookedMesh out{};
        if(!scene.scene || !scene.scene->mRootNode)
            return out;

        ctx.out = &out;

        build_materials(scene, ctx);
        visit_node(scene.scene->mRootNode, aiMatrix4x4(), ctx);

        out.axisInfo = ctx.dstAxis;
        out.aabb = computeAABB(out);
        return out;
    }

    #pragma pack(push,1)
    struct FileHeaderV2 {
        char     magic[8];   // "MBMESH\2"
        uint32_t version;    // 2
        uint32_t reserved;   // 0
    };
    #pragma pack(pop)

    // write helpers
    template<class T>
    inline void wpod(std::vector<uint8_t>& buf, const T& v){
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&v);
        buf.insert(buf.end(), p, p+sizeof(T));
    }
    inline void wbytes(std::vector<uint8_t>& buf, const void* data, size_t n){
        const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
        buf.insert(buf.end(), p, p+n);
    }
    inline void wstr(std::vector<uint8_t>& buf, std::string_view s){
        uint32_t n = static_cast<uint32_t>(s.size());
        wpod(buf, n);
        if(n)
            wbytes(buf, s.data(), n);
    }

    // read helpers
    struct R{
        const uint8_t* p;
        const uint8_t* e;

        inline bool ok() const { return p<=e; }

        template<class T>
        bool rpod(T& out){
            if(static_cast<size_t>(e-p) < sizeof(T))
                return false;
            std::memcpy(&out, p, sizeof(T)); p += sizeof(T);
                return true;
        }
        bool rbytes(void* dst, size_t n){
            if(static_cast<size_t>(e-p) < n)
                return false;
            std::memcpy(dst,p,n); p+=n;
                return true;
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

    struct ExtCandidate{
        std::string normRelPath;
        std::string stem;
        std::string parentDirNorm;
        fs::path    fullPath;
    };
    struct EmbCandidate{
        uint64_t contentHash = 0;
        std::string hint;
        const aiTexture* tex = nullptr;
        const aiScene* owner = nullptr;
        TextureUsage usage = TextureUsage::BaseColor;
    };
    struct BatchTables {
        std::unordered_set<std::string> existingNames;
        std::unordered_map<std::string, std::string> finalNameByExternal;
        std::unordered_map<uint64_t,   std::string> finalNameByEmbedded;
        std::unordered_map<std::string, ExtCandidate> repByExternal;
        std::unordered_map<uint64_t,   EmbCandidate> repByEmbedded;
    };

    void batch_scan_existing(const fs::path& texturesDir, BatchTables& bt){
        std::error_code ec;
        if(!fs::exists(texturesDir, ec))
            return;
        for(auto& e: fs::directory_iterator(texturesDir, ec)){
            if(e.is_regular_file(ec) && e.path().extension()==".ktx2"){
                bt.existingNames.insert(e.path().filename().string());
            }
        }
    }

    void batch_collect_from_scene(
        const RawScene& scene, BuildCtx& ctx,
        std::unordered_map<std::string, std::vector<ExtCandidate>>& groupsByStem,
        BatchTables& bt
    ){
        if(!ctx.aiScene || !ctx.aiScene->HasMaterials())
            return;

        for(unsigned i=0; i<ctx.aiScene->mNumMaterials; ++i){
            const aiMaterial* m = ctx.aiScene->mMaterials[i];
            aiString aiName = m->GetName();
            std::string matName = (aiName.length>0) ?
                std::string(aiName.C_Str()) : std::format("Material{}", i);

            auto handleOne = [&](aiTextureType t, TextureUsage usage){
                auto n = m->GetTextureCount(t);
                for(unsigned ti=0; ti<n; ++ti){
                    aiString texPath; if(m->GetTexture(t, ti, &texPath) != aiReturn_SUCCESS)
                        continue;
                    TexResolve R = resolve_uri(scene, texPath);

                    if(R.embedded){
                        const aiTexture* tex = (R.index>=0 && R.index < int(scene.scene->mNumTextures)) ?
                            scene.scene->mTextures[R.index] : nullptr;
                        uint64_t h = content_hash(tex);
                        if(!h)
                            continue;

                        std::string hint = sanitize(std::format("{}_{}_{}_{}",
                            ctx.sceneStem, matName, usageToSlotName(usage), ti));
                        if(!bt.repByEmbedded.count(h)){
                            bt.repByEmbedded[h] = EmbCandidate{h, hint, tex, scene.scene.get(), usage};
                        }
                    }else {
                        fs::path rel = fs::path(R.src).lexically_relative(scene.baseDir);
                        std::string norm = normalizePathString(rel);
                        fs::path p(norm);
                        std::string stem = p.stem().string();
                        std::string parent = normalizePathString(p.parent_path());
                        if(!bt.repByExternal.count(norm)){
                            bt.repByExternal[norm] = ExtCandidate{norm, stem, parent, R.src};
                        }
                        groupsByStem[stem].push_back(
                            ExtCandidate{norm, stem, parent, R.src});
                    }
                }
            };

            handleOne(aiTextureType_BASE_COLOR,        TextureUsage::BaseColor);
            handleOne(aiTextureType_DIFFUSE,           TextureUsage::BaseColor);
            handleOne(aiTextureType_NORMALS,           TextureUsage::Normal);
            handleOne(aiTextureType_NORMAL_CAMERA,     TextureUsage::Normal);
            handleOne(aiTextureType_METALNESS,         TextureUsage::MR);
            handleOne(aiTextureType_DIFFUSE_ROUGHNESS, TextureUsage::MR);
            handleOne(aiTextureType_UNKNOWN,           TextureUsage::MR);
            handleOne(aiTextureType_EMISSIVE,          TextureUsage::Emissive);
        }
    }

    void batch_decide_final_names(
        const fs::path& /*texturesDir*/,
        std::unordered_map<std::string, std::vector<ExtCandidate>>& groupsByStem,
        BatchTables& bt
    ){
        // 외부: stem 단일 → stem.ktx2, 다중 → stem_h8(parentDir).ktx2
        for(auto& [stem, vec]: groupsByStem){
            if(vec.size()==1){
                const auto& c = vec[0];
                bt.finalNameByExternal[c.normRelPath]
                    = sanitize(stem) + ".ktx2";
            }else {
                for(const auto& c: vec){
                    uint64_t dirh = fnv1a64_str(c.parentDirNorm);
                    bt.finalNameByExternal[c.normRelPath]
                        = sanitize(c.stem) + "_" + hex8(dirh) + ".ktx2";
                }
            }
        }

        // 디스크/배치 충돌시 identity 해시 추가
        std::unordered_map<std::string,int> nameCount;

        for(const auto& kv: bt.finalNameByExternal)
            nameCount[kv.second]++;
        for(auto& kv: bt.finalNameByExternal){
            auto& decided = kv.second;
            bool diskHit = bt.existingNames.count(decided)>0;
            bool multi   = nameCount[decided]>1;
            if(diskHit || multi){
                uint64_t idh = fnv1a64_str(kv.first);
                decided = fs::path(decided).stem().string() + "_" + hex8(idh) + ".ktx2";
            }
        }

        // 임베드: hint__h8(hash).ktx2
        for(const auto& [h, rep] : bt.repByEmbedded){
            bt.finalNameByEmbedded[h] = rep.hint + "__" + hex8(h) + ".ktx2";
        }
    }

    void batch_cook_missing(const fs::path& texturesDir, BatchTables& bt){
        std::error_code ec;
        fs::create_directories(texturesDir, ec);

        for(const auto& [normRel, rep]: bt.repByExternal){
            auto it = bt.finalNameByExternal.find(normRel);
            if(it==bt.finalNameByExternal.end())
                continue;

            fs::path out = texturesDir / it->second;
            if(fs::exists(out, ec))
                continue;
            auto kp = make_ktx_params(TextureUsage::BaseColor);
            int rc = cook_image_to_ktx2_file(rep.fullPath.string().c_str(),
                out.string().c_str(), &kp);
            if(rc!=0){
                std::println(std::cerr, "[warn] ktx2 cook failed (external): {} -> {}",
                    rep.fullPath.string(), out.string());
            }
        }

        for(const auto& [h, rep]: bt.repByEmbedded){
            auto it = bt.finalNameByEmbedded.find(h);
            if(it==bt.finalNameByEmbedded.end())
                continue;
            
            fs::path out = texturesDir / it->second;
            if(fs::exists(out, ec))
                continue;

            fs::path hint = texturesDir / std::format("__embed_tmp_{}", hex8(h));
            fs::path real;
            if(!export_assimp_embedded_to_temp(rep.tex, hint, real)){
                std::println(std::cerr, "[warn] export embedded failed");
                continue;
            }
            auto kp = make_ktx_params(rep.usage);
            int rc = cook_image_to_ktx2_file(real.string().c_str(),
                out.string().c_str(), &kp);
            std::error_code rec; fs::remove(real, rec);
            if(rc!=0){
                std::println(std::cerr, "[warn] ktx2 cook failed (embedded): {} -> {}",
                    real.string(), out.string());
            }
        }
    }
}

auto Asset::importMeshFiles(
    std::span<const fs::path> inputPaths,
    const CookOptions& options
)->CookedMesh{
    // 1) 모두 로드
    std::vector<RawScene> scenes; scenes.reserve(inputPaths.size());
    for(const auto& p : inputPaths){
        scenes.emplace_back(import(p));
    }

    // 2) 배치 테이블 준비 + 디스크 스캔
    BatchTables bt{};
    const fs::path texturesDir = "asset/textures";
    batch_scan_existing(texturesDir, bt);

    // 3) 후보 수집
    std::unordered_map<std::string, std::vector<ExtCandidate>> groupsByStem;
    for(const auto& rs : scenes){
        BuildCtx ctx = make_ctx(rs, options.axes);
        batch_collect_from_scene(rs, ctx, groupsByStem, bt);
    }

    // 4) 이름 확정
    batch_decide_final_names(texturesDir, groupsByStem, bt);

    // 5) 대표만 변환 (디스크에 없을 때만)
    batch_cook_missing(texturesDir, bt);

    // 6) 각 씬 빌드(URI는 결정된 이름을 참조)
    CookedMesh merged{};
    merged.axisInfo = options.axes;

    for(const auto& rs : scenes){
        BuildCtx ctx = make_ctx(rs, options.axes);
        ctx.finalNameByExternal = &bt.finalNameByExternal;
        ctx.finalNameByEmbedded = &bt.finalNameByEmbedded;
        CookedMesh one = buildMesh(rs, ctx);

        // 머지
        merged.submeshes.insert(merged.submeshes.end(),
                                std::make_move_iterator(one.submeshes.begin()),
                                std::make_move_iterator(one.submeshes.end()));
        for(auto& kv : one.materials){
            merged.materials.emplace(std::move(kv.first), std::move(kv.second));
        }
    }
    merged.aabb = computeAABB(merged);
    return merged;
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

auto Asset::extractHeader(const CookedMesh& cooked) -> Header {
    // Provide a best-effort synthetic header for legacy callers that still expect it.
    Header H{};
    const char magic[] = "MBMESH\2";
    std::memcpy(H.magic, magic, sizeof(H.magic));
    H.version = 0;
    H.headerSize = sizeof(Header);
    // We no longer use on-disk tables; fill some counts for reference only.
    H.numSubmesh = cooked.submeshes.size();
    H.numMaterial = cooked.materials.size();
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
        wpod(buf, vc);
        if (vc)
            wbytes(buf, sm.vertices.data(), static_cast<size_t>(vc) * sizeof(Vertex));
        wpod(buf, ic);
        if (ic)
            wbytes(buf, sm.indices.data(), static_cast<size_t>(ic) * sizeof(uint32_t));
    }

    uint32_t matCount = static_cast<uint32_t>(cooked.materials.size());
    wpod(buf, matCount);

    for(const auto& kv : cooked.materials){
        const std::string& matName = kv.first;
        const CookedMaterial& m = kv.second;
        wstr(buf, matName);
        {
            uint32_t mt = static_cast<uint32_t>(m.type);
            wpod(buf, mt);
        }
        {
            uint32_t texCount = static_cast<uint32_t>(m.textures.size());
            wpod(buf, texCount);
        }

        for(const auto& tv : m.textures){
            const std::string& slotKey = tv.first;
            const CookedTexture& t = tv.second;
            wstr(buf, slotKey);
            {
                uint32_t usage = static_cast<uint32_t>(t.usage);
                wpod(buf, usage);
            }
            wpod(buf, t.flags);            // uint16_t
            wpod(buf, static_cast<uint16_t>(0)); // pad
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

    uint32_t smCount=0;
    if(!r.rpod(smCount))
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

    uint32_t matCount=0;
    if(!r.rpod(matCount))
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
