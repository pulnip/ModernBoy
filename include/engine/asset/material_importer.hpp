#ifndef MODERNBOY_ASSET_MATERIALIMPORTER_HPP
#define MODERNBOY_ASSET_MATERIALIMPORTER_HPP

#include <filesystem>
#include <format>
#include <vector>
#include "core/math/type.hpp"
#include "engine/asset/asset_format.hpp"

namespace ModernBoy::Asset
{
    struct BaseColorImage{
        std::vector<uint8_t> pixels;
        int32_t width, height;
    };

    auto importImageFile(
        const std::filesystem::path& inputPath
    ) -> BaseColorImage;
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_MATERIALIMPORTER_HPP