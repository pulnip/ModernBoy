#ifndef MODERNBOY_ASSET_MESHIMPORTER_HPP
#define MODERNBOY_ASSET_MESHIMPORTER_HPP

#include <filesystem>
#include "core/math/type.hpp"
#include "engine/asset/asset_format.hpp"

namespace ModernBoy::Asset
{
    struct CookOptions{
        AxisInfo axes = {};
        bool keepSkin = true;
        bool generateTangents = true;
    };

    auto importModelFile(
        const std::filesystem::path& inputPath,
        const CookOptions& options = {}
    )->CookedMesh;

    auto serialize(const CookedMesh&,
        const std::filesystem::path& outputPath)->void;
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_MESHIMPORTER_HPP