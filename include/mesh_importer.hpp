#ifndef MODERNBOY_MESH_IMPORTER_HPP
#define MODERNBOY_MESH_IMPORTER_HPP

#include <string>
#include <raw_resource.hpp>

namespace ModernBoy{
    // RawMesh for shader test
    RawMeshes createTriangle();
    RawMeshes createRectangle();
    RawMeshes createCube();

    RawMeshes fromFbx(const std::string& fileName);
}

#endif // MODERNBOY_MESH_IMPORTER_HPP