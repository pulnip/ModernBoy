#ifndef MODERNBOY_IMPORTER_HPP
#define MODERNBOY_IMPORTER_HPP

#include <string>
#include "raw_resource.hpp"

namespace ModernBoy
{
    template<typename Resource>
    Resource import(const std::string& fileName);
    RawMeshes importMesh(const std::string& fileName);
} // namespace ModernBoy

#endif // MODERNBOY_IMPORTER_HPP