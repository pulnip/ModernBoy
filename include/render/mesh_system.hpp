#ifndef __INC_MESH_SYSTEM_HPP
#define __INC_MESH_SYSTEM_HPP

#include <algorithm>
#include <execution>
#include <functional>
#include <vector>
#include "mesh_component.hpp"

namespace ModernBoy
{
    template<typename Mesh>
    class MeshSystem{
    private:
        using Component = MeshComponent<Mesh>;

        std::vector<Component> components;

    public:
        std::vector<std::reference_wrapper<const Component>> getAllVisible() const{
            std::vector<std::reference_wrapper<const Component>> wrapped;

            std::for_each(std::execution::par,
                components.cbegin(), components.cend(),
                [&wrapped](const auto& comp){
                    wrapped.emplace_back(std::cref(comp));
                }
            );

            return wrapped;
        }
    };
} // namespace ModernBoy

#endif // __INC_MESH_SYSTEM_HPP