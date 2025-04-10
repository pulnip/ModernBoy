#ifndef __INC_COMPONENT_SYSTEM_HPP
#define __INC_COMPONENT_SYSTEM_HPP

#include <algorithm>
#include <execution>
#include <functional>
#include <vector>

namespace ModernBoy
{
    template<typename Component>
    class ComponentSystem{
    private:
        std::vector<Component> components;

    public:
        void create(const Component& component){
            components.emplace_back(component);
        }

        auto getAll() const{
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

#endif // __INC_COMPONENT_SYSTEM_HPP