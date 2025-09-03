#include "asset/material_importer.hpp"
#include <stb_image.h>

using namespace ModernBoy;
using namespace ModernBoy::Asset;

auto Asset::importImageFile(
    const std::filesystem::path& inputPath
) -> BaseColorImage{
    int width, height, channels;
    auto data = stbi_load(inputPath.string().c_str(),
        &width, &height, &channels, STBI_rgb_alpha);
    if(!data){
        throw std::runtime_error("Failed to load image: " + inputPath.string());
    }

    BaseColorImage image;
    image.width = width;
    image.height = height;
    // copy to std::vector<uint8_t>
    image.pixels.assign(data, data + width * height * 4);

    stbi_image_free(data);
    return image;
}