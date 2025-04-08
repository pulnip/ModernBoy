#ifndef __INC_MESH_COMPONENT_HPP
#define __INC_MESH_COMPONENT_HPP

#include "resource_handle.hpp"

namespace ModernBoy
{
    template<typename Mesh>
    struct MeshComponent{
        EntityID owner;
        ResourceHandle<Mesh> meshHandle;

        bool visible = true;
    };
} // namespace ModernBoy

#endif // __INC_MESH_COMPONENT_HPP