#ifndef MODERNBOY_ASSET_ASSETFORMAT_HPP
#define MODERNBOY_ASSET_ASSETFORMAT_HPP

#include <cstdint>
#include <vector>
#include "core/math/type.hpp"
#include "raw_resource.hpp"

namespace ModernBoy::Asset
{
    // naming rule
    // ~Stride: element byte size
    // ~ByteOffset: byte offset from file start
    // num~: number of total element (=container.size())
    // ~ByteSize: Container byte size
    // ~SectionIndex: element index of its container
    // ~Count: number of element

    struct Header{
        char magic[8] = "MBMESH\1";
        uint32_t version = 1;
        uint32_t headerSize = sizeof(Header);


        // equivalent to Submesh Slot Size [bytes]
        uint32_t submeshTableStride = 0;
        uint32_t submeshTableByteOffset = 0;
        uint32_t numSubmesh = 0;
        uint32_t submeshTableByteSize = 0;

        uint32_t materialTableStride = 0;
        uint32_t materialTableByteOffset = 0;
        uint32_t numMaterial = 0;
        uint32_t materialTableByteSize = 0;

        uint32_t materialNameByteOffset = 0;
        uint32_t materialNameByteSize = 0;

        uint32_t textureInfoTableStride = 0;
        uint32_t textureInfoTableByteOffset = 0;
        uint32_t numTexture = 0;
        uint32_t textureInfoTableByteSize = 0;

        uint32_t verticesSectionStride = 0;
        uint32_t verticesSectionByteOffset = 0;
        uint32_t numVertex = 0;
        uint32_t verticesSectionByteSize = 0;

        uint32_t indicesSectionStride  = 0;
        uint32_t indicesSectionByteOffset = 0;
        uint32_t numIndex = 0;
        uint32_t indicesSectionByteSize = 0;

        uint32_t pixelsSectionByteOffset = 0;
        uint32_t pixelsSectionByteSize = 0;
    };

    struct AxisInfo {
        enum Hand: uint8_t{ RH=0, LH=1 } hand = LH;
        uint8_t up = 'Y';
        uint8_t forward = 'Z';
        uint8_t flipV = true;
        // 1.0f for 1[meter]
        float unit = 0.01f;
    };

    struct AABB{
        Vec3 min, max;
    };

    struct SubmeshInfo{
        uint32_t entrySize;
        uint32_t verticesSectionIndex = 0;
        uint32_t vertexCount = 0;
        uint32_t indicesSectionIndex = 0;
        uint32_t indexCount = 0;
        uint32_t materialTableIndex = 0;
    };

    enum MaterialType: uint32_t{
        Unlit = 0,
        PBR = 1,
    };

    struct MaterialInfo{
        uint32_t entrySize;
        MaterialType type;
        uint32_t textureInfoTableIndex = 0;
        uint32_t textureCount = 0;
        uint32_t nameByteOffset = 0;
        uint32_t nameByteSize = 0;
    };

    enum TextureUsage: uint16_t{
        BaseColor = 0,
        Normal = 1,
        MR = 2,
        Emissive = 3,
    };

    enum TextureFormat: uint16_t{
        R8G8B8A8_Unorm,
        R8G8B8A8_SRGB
    };

    struct TextureInfo{
        uint32_t entrySize;
        TextureUsage usage;
        TextureFormat format;
        uint16_t width, height;
        uint32_t pixelSectionIndex;
        // equivalent to Pixel Byte Size
        uint32_t pixelCount;
    };

    struct CookedMesh{
        // mesh metadata (16 byte aligned on file.)
        Header header;
        AxisInfo axisInfo;
        AABB aabb;
        // information Tables (16 byte aligned on file.)
        std::vector<SubmeshInfo> submeshInfoTable;
        std::vector<MaterialInfo> materialInfoTable;
        std::vector<std::string> materialNameTable;
        std::vector<TextureInfo> textureInfoTable;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<uint8_t> pixels;
    };
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_ASSETFORMAT_HPP