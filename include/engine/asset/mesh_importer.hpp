#ifndef MODERNBOY_ASSET_MESHIMPORTER_HPP
#define MODERNBOY_ASSET_MESHIMPORTER_HPP

#include <filesystem>
#include <format>
#include "core/math/type.hpp"
#include "engine/asset/asset_format.hpp"

namespace ModernBoy::Asset
{
    struct CookOptions{
        AxisInfo axes = {};
        bool keepSkin = true;
        bool generateTangents = true;
    };

    // convert .fbx, ... to .mbmesh
    auto importMeshFile(
        const std::filesystem::path& inputPath,
        const CookOptions& options = {}
    )->CookedMesh;
    auto extractHeader(const CookedMesh& cooked)->Header;

    auto serialize(const CookedMesh&,
        const std::filesystem::path& outputPath)->void;

    // --- Test-friendly helpers ---
    // Serialize to an in-memory buffer (no filesystem needed)
    auto serializeToBuffer(const CookedMesh&) -> std::vector<uint8_t>;

    // Load a CookedMesh from an in-memory buffer
    auto loadFromBuffer(const std::vector<uint8_t>&) -> CookedMesh;

    // load .mbmesh to memory
    auto loadMeshFile(
        const std::filesystem::path& inputPath
    )->CookedMesh;

    void printLoadedMesh(const CookedMesh&);

    // Embeded Mesh for shader test
    // ToDo. change to CookedMesh or ...
    RawMesh createTriangle();
    RawMesh createRectangle();
    RawMesh createCube();
    RawMesh createSphere(float radius=1.0f,
        int numSlices=32, int numStacks=16);

    template<typename Resource>
    Resource importEmbedded(const std::string& fileName){
        RawMesh rawMesh;

        if(fileName.compare("triangle") == 0)
            rawMesh = createTriangle();
        else if(fileName.compare("tectangle") == 0)
            rawMesh = createRectangle();
        else if(fileName.compare("cube") == 0)
            rawMesh = createCube();
        else if(fileName.compare("sphere") == 0)
            rawMesh = createSphere();
        else
            throw std::runtime_error(
                std::format("Not Implemented Embedded Type: {}",
                fileName)
            );
    
        return rawMesh;
    }
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_MESHIMPORTER_HPP