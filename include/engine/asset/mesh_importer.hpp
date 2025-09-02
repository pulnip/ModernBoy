#ifndef MODERNBOY_ASSET_MESHIMPORTER_HPP
#define MODERNBOY_ASSET_MESHIMPORTER_HPP

#include <filesystem>
#include <format>
#include <span>
#include "core/math/type.hpp"
#include "engine/asset/asset_format.hpp"

namespace ModernBoy::Asset
{
    struct CookOptions{
        AxisInfo axes = {};
        bool keepSkin = true;
        bool generateTangents = true;
    };

    // Convert .fbx, ... to .mbmesh (simple container format)
    auto importMeshFiles(
        std::span<const std::filesystem::path> inputPaths,
        const CookOptions& options = {}
    )->CookedMesh;

    // Extract a synthetic header for .mbmesh container (v2, simple format)
    auto extractHeader(const CookedMesh& cooked)->Header;

    // Write .mbmesh container (simple format) to file
    auto serialize(const CookedMesh&,
        const std::filesystem::path& outputPath)->void;

    // --- Test-friendly helpers for the simple container format ---
    // Serialize to an in-memory buffer (simple .mbmesh container)
    auto serializeToBuffer(const CookedMesh&) -> std::vector<uint8_t>;

    // Load a CookedMesh from an in-memory buffer (simple .mbmesh container)
    auto loadFromBuffer(const std::vector<uint8_t>&) -> CookedMesh;

    // Load .mbmesh (simple container format) from file
    auto loadMeshFile(
        const std::filesystem::path& inputPath
    )->CookedMesh;

    // Print summary of loaded CookedMesh (simple container format)
    void printLoadedMesh(const CookedMesh&);

    struct EmbeddedMesh{
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    // Embeded Mesh for shader test
    auto loadEmbeddedMesh(const std::string& name)->EmbeddedMesh;
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_MESHIMPORTER_HPP