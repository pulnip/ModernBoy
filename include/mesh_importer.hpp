#ifndef MODERNBOY_MESH_IMPORTER_HPP
#define MODERNBOY_MESH_IMPORTER_HPP

#include <string>
#include <raw_resource.hpp>
#include <resource_importer.hpp>

namespace ModernBoy{
    class MeshImporter{
    public:
        RawMeshes import(const std::string& fileName);
    };
    static_assert(ResourceImporter<MeshImporter, RawMeshes>);
}

#endif // MODERNBOY_MESH_IMPORTER_HPP