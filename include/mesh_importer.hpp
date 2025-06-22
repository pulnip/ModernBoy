#ifndef MODERNBOY_MESH_IMPORTER_HPP
#define MODERNBOY_MESH_IMPORTER_HPP

#include <string>
#include "raw_resource.hpp"

namespace ModernBoy
{
    class MeshImporter{
    public:
        RawMeshes import(const std::string& fileName);
    };
} // namespace ModernBoy

#endif // MODERNBOY_MESH_IMPORTER_HPP