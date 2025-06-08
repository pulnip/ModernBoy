#ifndef MODERNBOY_MODEL_LOADER
#define MODERNBOY_MODEL_LOADER

#include <string>
#include "raw_resource.hpp"

namespace ModernBoy
{
    RawMeshes loadMeshes(const std::string& fileName);
    RawTexture loadTexture(const std::string& fileName);
}

#endif // MODERNBOY_MODEL_LOADER