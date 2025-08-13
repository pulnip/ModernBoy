#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <print>
#include <algorithm>
#include <cstring>
#include <limits>
#include <cmath>

namespace fs = std::filesystem;

// The viewer mirrors the writer layout in mesh_converter.cpp
// Header layout WITH magic/version:
struct Header {
    char     magic[8];        // e.g., "MBMESH\x01\0"
    uint32_t version = 0;
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    uint32_t submeshCount = 0;
    uint32_t materialCount = 0;
    uint32_t vertexStride = 0; // bytes per-vertex in file
    uint32_t indexStride  = 0; // bytes per-index in file (usually 4)
};

struct Submesh {
    uint32_t indexOffset = 0;
    uint32_t indexCount = 0;
    uint32_t materialIndex = 0;
};

// Shader-expected vertex layout
struct Vertex {
    float px, py, pz;        // position
    float nx, ny, nz;        // normal
    float u0, v0;            // uv0
    float tx, ty, tz;        // tangent
};

struct AABB { float min[3], max[3]; };

static bool ReadAll(const fs::path& p, std::vector<std::byte>& out){
    std::ifstream ifs(p, std::ios::binary);
    if(!ifs) return false;
    ifs.seekg(0, std::ios::end);
    const auto size = static_cast<size_t>(ifs.tellg());
    ifs.seekg(0, std::ios::beg);
    out.resize(size);
    if(size) ifs.read(reinterpret_cast<char*>(out.data()), size);
    return !!ifs;
}

static const char* HumanSize(uint64_t n){
    static thread_local char buf[64];
    const char* units[] = {"B","KB","MB","GB"};
    double v = (double)n; int u=0; while(v>=1024.0 && u<3){ v/=1024.0; ++u; }
    std::snprintf(buf, sizeof(buf), "%.2f %s", v, units[u]);
    return buf;
}

static std::string MagicAscii(const char magic[8]){
    char buf[9];
    for(int i=0;i<8;++i){
        char c = magic[i];
        buf[i] = (c>=32 && c<=126) ? c : '.';
    }
    buf[8] = '\0';
    return std::string(buf);
}

static std::string MagicHex(const char magic[8]){
    char buf[3*8+1];
    int k=0; for(int i=0;i<8;++i){
        unsigned char b = static_cast<unsigned char>(magic[i]);
        std::snprintf(buf+k, 4, "%02X ", b); k+=3;
    }
    buf[k?k-1:0] = '\0';
    return std::string(buf);
}

static void PrintTreeLine(const std::string& prefix, const std::string& label){
    std::println("{}{}", prefix, label);
}

static void ComputeAABBFromVertices(const std::byte* verts, uint32_t count, uint32_t stride, AABB& aabb){
    aabb.min[0]=aabb.min[1]=aabb.min[2]= std::numeric_limits<float>::infinity();
    aabb.max[0]=aabb.max[1]=aabb.max[2]=-std::numeric_limits<float>::infinity();
    for(uint32_t i=0;i<count;++i){
        const std::byte* p = verts + size_t(i)*stride;
        float x,y,z;
        std::memcpy(&x, p + 0,               sizeof(float));
        std::memcpy(&y, p + sizeof(float),   sizeof(float));
        std::memcpy(&z, p + sizeof(float)*2, sizeof(float));
        aabb.min[0] = std::min(aabb.min[0], x);
        aabb.min[1] = std::min(aabb.min[1], y);
        aabb.min[2] = std::min(aabb.min[2], z);
        aabb.max[0] = std::max(aabb.max[0], x);
        aabb.max[1] = std::max(aabb.max[1], y);
        aabb.max[2] = std::max(aabb.max[2], z);
    }
}

struct IndexStats {
    uint32_t minIndex = UINT32_MAX;
    uint32_t maxIndex = 0;
    uint64_t outOfRange = 0;
    uint64_t degenerate = 0; // triangles with duplicated verts or near-zero area
};

struct AttrStats {
    // Normals and tangents
    double nAvg=0.0, nMin=1e9, nMax=-1e9;
    double tAvg=0.0, tMin=1e9, tMax=-1e9;
    double dotNTMaxAbs=0.0;
    uint64_t nNaN=0, tNaN=0;
};

struct UVStats {
    float uMin= std::numeric_limits<float>::infinity();
    float uMax=-std::numeric_limits<float>::infinity();
    float vMin= std::numeric_limits<float>::infinity();
    float vMax=-std::numeric_limits<float>::infinity();
    uint64_t nonFinite=0;
};

static inline const std::byte* VertexAt(const std::byte* base, uint32_t i, uint32_t stride){
    return base + size_t(i) * size_t(stride);
}

static inline void ReadPos(const std::byte* p, float& x, float& y, float& z){
    std::memcpy(&x, p + 0,               sizeof(float));
    std::memcpy(&y, p + sizeof(float),   sizeof(float));
    std::memcpy(&z, p + sizeof(float)*2, sizeof(float));
}

static IndexStats ComputeIndexStats(const uint32_t* idx, uint32_t indexCount, uint32_t vertexCount,
                                    const std::byte* verts, uint32_t stride){
    IndexStats s{};
    if(indexCount==0) return s;
    for(uint32_t i=0;i<indexCount;++i){
        uint32_t v = idx[i];
        s.minIndex = std::min(s.minIndex, v);
        s.maxIndex = std::max(s.maxIndex, v);
        if(v >= vertexCount) ++s.outOfRange;
    }
    // Degenerate: duplicates or near-zero area
    const uint32_t triCount = indexCount / 3;
    const bool canReadPos = (stride >= sizeof(float)*3) && (verts != nullptr);
    for(uint32_t t=0;t<triCount;++t){
        uint32_t i0 = idx[t*3+0], i1 = idx[t*3+1], i2 = idx[t*3+2];
        if(i0==i1 || i1==i2 || i2==i0){ ++s.degenerate; continue; }
        if(!canReadPos) continue;
        if(i0>=vertexCount || i1>=vertexCount || i2>=vertexCount) continue;
        float x0,y0,z0,x1,y1,z1,x2,y2,z2;
        ReadPos(VertexAt(verts,i0,stride), x0,y0,z0);
        ReadPos(VertexAt(verts,i1,stride), x1,y1,z1);
        ReadPos(VertexAt(verts,i2,stride), x2,y2,z2);
        const float ax = x1-x0, ay = y1-y0, az = z1-z0;
        const float bx = x2-x0, by = y2-y0, bz = z2-z0;
        const float cx = ay*bz - az*by;
        const float cy = az*bx - ax*bz;
        const float cz = ax*by - ay*bx;
        const float area2 = cx*cx + cy*cy + cz*cz; // 4*area^2
        if(area2 <= 1e-12f) ++s.degenerate;
    }
    return s;
}

static AttrStats ComputeAttrStats(const std::byte* verts, uint32_t vcount, uint32_t stride){
    AttrStats a{};
    if(vcount==0) return a;
    // Offsets for our known layout: pos(0), normal(12), uv(24), tangent(32)
    const size_t nOff = sizeof(float)*3;
    const size_t tOff = sizeof(float)*8; // after pos(3) + normal(3) + uv(2)
    for(uint32_t i=0;i<vcount;++i){
        const std::byte* p = VertexAt(verts,i,stride);
        float nx,ny,nz, tx,ty,tz;
        std::memcpy(&nx, p + nOff + 0*sizeof(float), sizeof(float));
        std::memcpy(&ny, p + nOff + 1*sizeof(float), sizeof(float));
        std::memcpy(&nz, p + nOff + 2*sizeof(float), sizeof(float));
        std::memcpy(&tx, p + tOff + 0*sizeof(float), sizeof(float));
        std::memcpy(&ty, p + tOff + 1*sizeof(float), sizeof(float));
        std::memcpy(&tz, p + tOff + 2*sizeof(float), sizeof(float));
        const bool nFin = std::isfinite(nx)&&std::isfinite(ny)&&std::isfinite(nz);
        const bool tFin = std::isfinite(tx)&&std::isfinite(ty)&&std::isfinite(tz);
        if(!nFin) ++a.nNaN; if(!tFin) ++a.tNaN;
        const double nl = nFin ? std::sqrt(double(nx)*nx + double(ny)*ny + double(nz)*nz) : 0.0;
        const double tl = tFin ? std::sqrt(double(tx)*tx + double(ty)*ty + double(tz)*tz) : 0.0;
        a.nAvg += nl; a.tAvg += tl;
        a.nMin = std::min(a.nMin, nl); a.nMax = std::max(a.nMax, nl);
        a.tMin = std::min(a.tMin, tl); a.tMax = std::max(a.tMax, tl);
        const double dotNT = (nFin && tFin) ? (nx*tx + ny*ty + nz*tz) : 0.0;
        a.dotNTMaxAbs = std::max(a.dotNTMaxAbs, std::abs(dotNT));
    }
    a.nAvg /= double(vcount);
    a.tAvg /= double(vcount);
    return a;
}

static UVStats ComputeUVStats(const std::byte* verts, uint32_t vcount, uint32_t stride){
    UVStats u{};
    if(vcount==0) return u;
    const size_t uvOff = sizeof(float)*6; // after pos(3) + normal(3)
    for(uint32_t i=0;i<vcount;++i){
        const std::byte* p = VertexAt(verts,i,stride);
        float uu,vv; std::memcpy(&uu, p + uvOff + 0*sizeof(float), sizeof(float));
        std::memcpy(&vv, p + uvOff + 1*sizeof(float), sizeof(float));
        if(!(std::isfinite(uu) && std::isfinite(vv))){ ++u.nonFinite; continue; }
        u.uMin = std::min(u.uMin, uu); u.uMax = std::max(u.uMax, uu);
        u.vMin = std::min(u.vMin, vv); u.vMax = std::max(u.vMax, vv);
    }
    return u;
}

static void ComputeBoundingSphere(const std::byte* verts, uint32_t vcount, uint32_t stride,
                                  const AABB& aabb, float& cx,float& cy,float& cz, float& radius){
    cx = (aabb.min[0]+aabb.max[0])*0.5f;
    cy = (aabb.min[1]+aabb.max[1])*0.5f;
    cz = (aabb.min[2]+aabb.max[2])*0.5f;
    radius = 0.0f;
    for(uint32_t i=0;i<vcount;++i){
        const std::byte* p = VertexAt(verts,i,stride);
        float x,y,z; ReadPos(p,x,y,z);
        const float dx=x-cx, dy=y-cy, dz=z-cz;
        radius = std::max(radius, std::sqrt(dx*dx+dy*dy+dz*dz));
    }
}

static int Run(const fs::path& target){
    std::vector<std::byte> blob;
    if(!ReadAll(target, blob)){
        std::println("[ERR] failed to read: {}", target.string());
        return 1;
    }
    if(blob.size() < sizeof(Header)){
        std::println("[ERR] file too small to be a .mbmesh: {}", blob.size());
        return 1;
    }

    // Parse header
    const Header* hdr = reinterpret_cast<const Header*>(blob.data());

    // Layout sizes
    const size_t headerBytes   = sizeof(Header);
    const size_t vertexBytes   = size_t(hdr->vertexCount) * size_t(hdr->vertexStride);
    const size_t indexBytes    = size_t(hdr->indexCount)  * size_t(hdr->indexStride ? hdr->indexStride : sizeof(uint32_t));
    const size_t submeshBytes  = size_t(hdr->submeshCount)* sizeof(Submesh);
    const size_t expectedBytes = headerBytes + vertexBytes + indexBytes + submeshBytes;

    bool sizeOk = (blob.size() == expectedBytes);

    // Pointers into the blob
    const std::byte* pVerts   = blob.data() + headerBytes;
    const std::byte* pIndices = pVerts + vertexBytes;
    const std::byte* pSubs    = pIndices + indexBytes;

    // Print tree
    PrintTreeLine("",        "mbmesh");
    PrintTreeLine("├─ ",     "header");
    PrintTreeLine("│  ├─ ",  std::format("magic         : '{}'  [{}]", MagicAscii(hdr->magic), MagicHex(hdr->magic)));
    PrintTreeLine("│  ├─ ",  std::format("version       : {}", hdr->version));
    PrintTreeLine("│  ├─ ",  std::format("vertexCount   : {}", hdr->vertexCount));
    PrintTreeLine("│  ├─ ",  std::format("indexCount    : {} ({} triangles)", hdr->indexCount, hdr->indexCount/3));
    PrintTreeLine("│  ├─ ",  std::format("submeshCount  : {}", hdr->submeshCount));
    PrintTreeLine("│  ├─ ",  std::format("materialCount : {}", hdr->materialCount));
    PrintTreeLine("│  └─ ",  std::format("vertexStride  : {} bytes{}; indexStride: {} bytes",
        hdr->vertexStride,
        (hdr->vertexStride==sizeof(Vertex)?" (matches local Vertex)":""),
        (hdr->indexStride ? hdr->indexStride : (uint32_t)sizeof(uint32_t))));

    PrintTreeLine("├─ ",     std::format("vertices [{}]  ({}):", hdr->vertexCount, HumanSize(vertexBytes)));
    PrintTreeLine("├─ ",     std::format("indices  [{}]  ({}):", hdr->indexCount, HumanSize(indexBytes)));
    PrintTreeLine("└─ ",     std::format("submeshes[{}]  ({}):", hdr->submeshCount, HumanSize(submeshBytes)));

    // Offsets block
    PrintTreeLine("├─ ", "offsets");
    const size_t offVerts = size_t(pVerts - blob.data());
    const size_t offInds  = size_t(pIndices - blob.data());
    const size_t offSubs  = size_t(pSubs - blob.data());
    PrintTreeLine("│  ├─ ", std::format("vertices : {:#010x}", (unsigned int)offVerts));
    PrintTreeLine("│  ├─ ", std::format("indices  : {:#010x}", (unsigned int)offInds));
    PrintTreeLine("│  └─ ", std::format("submeshes: {:#010x}", (unsigned int)offSubs));

    // Submesh listing
    if(hdr->submeshCount){
        const Submesh* subs = reinterpret_cast<const Submesh*>(pSubs);
        for(uint32_t i=0;i<hdr->submeshCount;++i){
            PrintTreeLine("   ├─ ", std::format("[{}] indexOffset={}, indexCount={}, materialIndex={}", i, subs[i].indexOffset, subs[i].indexCount, subs[i].materialIndex));
        }
    }

    // Geometry/index validation
    IndexStats istat{};
    if(hdr->indexStride == 2){
        // Convert U16 indices to U32 scratch for stats
        const uint16_t* idx16 = reinterpret_cast<const uint16_t*>(pIndices);
        std::vector<uint32_t> tmp(hdr->indexCount);
        for(uint32_t i=0;i<hdr->indexCount;++i) tmp[i] = idx16[i];
        istat = ComputeIndexStats(tmp.data(), hdr->indexCount, hdr->vertexCount, pVerts, hdr->vertexStride);
    }else if(hdr->indexStride == 0 || hdr->indexStride == 4){
        const uint32_t* idx32 = reinterpret_cast<const uint32_t*>(pIndices);
        istat = ComputeIndexStats(idx32, hdr->indexCount, hdr->vertexCount, pVerts, hdr->vertexStride);
    }else{
        PrintTreeLine("├─ ", std::string("geometry"));
        PrintTreeLine("│  └─ ", std::format("unsupported indexStride: {}", hdr->indexStride));
    }
    const bool u16Capable = (istat.maxIndex <= 65535u);
    PrintTreeLine("├─ ", std::format("geometry"));
    PrintTreeLine("│  ├─ ", std::format("indexRange       : [{}..{}]{}",
        (istat.minIndex==UINT32_MAX?0:istat.minIndex), istat.maxIndex,
        u16Capable?" (U16-capable)":""));
    PrintTreeLine("│  ├─ ", std::format("outOfRangeIndices: {}", istat.outOfRange));
    PrintTreeLine("│  └─ ", std::format("degenerateTris   : {}", istat.degenerate));

    // Attribute and bounds analysis (requires our known vertex layout)
    if(hdr->vertexStride == sizeof(Vertex) && hdr->vertexCount){
        // Bounds
        AABB aabb; ComputeAABBFromVertices(pVerts, hdr->vertexCount, hdr->vertexStride, aabb);
        float cx,cy,cz, rad; ComputeBoundingSphere(pVerts, hdr->vertexCount, hdr->vertexStride, aabb, cx,cy,cz, rad);
        PrintTreeLine("├─ ", std::format("bounds"));
        PrintTreeLine("│  ├─ ", std::format("AABB   : min({:.4f},{:.4f},{:.4f}) max({:.4f},{:.4f},{:.4f})",
            aabb.min[0],aabb.min[1],aabb.min[2], aabb.max[0],aabb.max[1],aabb.max[2]));
        PrintTreeLine("│  └─ ", std::format("sphere : center({:.4f},{:.4f},{:.4f}) radius={:.4f}", cx,cy,cz, rad));

        // Attributes
        AttrStats as = ComputeAttrStats(pVerts, hdr->vertexCount, hdr->vertexStride);
        UVStats   us = ComputeUVStats(pVerts, hdr->vertexCount, hdr->vertexStride);
        PrintTreeLine("├─ ", std::format("attributes"));
        PrintTreeLine("│  ├─ ", std::format("|N| length  : avg={:.3f} min={:.3f} max={:.3f} (NaN={})",
            as.nAvg, as.nMin, as.nMax, as.nNaN));
        PrintTreeLine("│  ├─ ", std::format("|T| length  : avg={:.3f} min={:.3f} max={:.3f} (NaN={})",
            as.tAvg, as.tMin, as.tMax, as.tNaN));
        PrintTreeLine("│  └─ ", std::format("max|dot(N,T)|: {:.4f}", as.dotNTMaxAbs));

        PrintTreeLine("├─ ", std::format("uv0"));
        PrintTreeLine("│  ├─ ", std::format("U range: [{:.4f} .. {:.4f}]", us.uMin, us.uMax));
        PrintTreeLine("│  └─ ", std::format("V range: [{:.4f} .. {:.4f}] (non-finite: {})", us.vMin, us.vMax, us.nonFinite));

        // Sample vertices for quick spot-check
        const Vertex* v = reinterpret_cast<const Vertex*>(pVerts);
        const uint32_t show = std::min<uint32_t>(hdr->vertexCount, 3);
        PrintTreeLine("", std::format("\nSample vertices ({}):", show));
        for(uint32_t i=0;i<show;++i){
            PrintTreeLine("  • ", std::format("v{}: P({:.4f},{:.4f},{:.4f}) N({:.3f},{:.3f},{:.3f}) UV({:.3f},{:.3f}) T({:.3f},{:.3f},{:.3f})",
                i, v[i].px,v[i].py,v[i].pz, v[i].nx,v[i].ny,v[i].nz, v[i].u0,v[i].v0, v[i].tx,v[i].ty,v[i].tz));
        }
    } else {
        PrintTreeLine("", std::format("\nAttributes/bounds: skipped (vertexStride={}, local Vertex={})", hdr->vertexStride, sizeof(Vertex)));
    }

    // Size sanity check at the end
    if(!sizeOk){
        PrintTreeLine("", std::format("\n[WARN] File size ({}) != expected layout size ({}). Format mismatch or version drift?",
            HumanSize(blob.size()), HumanSize(expectedBytes)));
    }

    return 0;
}

int main(int argc, char* argv[]){
    if(argc < 3){
        std::println("usage: {} --target <file.mbmesh>", argv[0]);
        return 1;
    }
    fs::path target;
    for(int i=1;i<argc;++i){
        std::string arg = argv[i];
        if((arg=="--target" || arg=="-t") && i+1<argc){ target = argv[++i]; }
    }
    if(target.empty()){
        std::println("[ERR] --target <file> is required");
        return 1;
    }
    if(!fs::exists(target) || !fs::is_regular_file(target)){
        std::println("[ERR] no such file: {}", target.string());
        return 1;
    }
    return Run(target);
}
