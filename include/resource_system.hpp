#ifndef __INC_RESOURCE_SYSTEM_HPP
#define __INC_RESOURCE_SYSTEM_HPP

#include <algorithm>
#include <execution>
#include <functional>
#include <vector>
#include "resource_component.hpp"

namespace ModernBoy
{
    template<typename T>
    class ResourceSystem{
    private:
        using Component = ResourceComponent<T>;
        using Handle = ResourceHandle<T>;

        std::vector<Component> components;

    public:
        void create(const Handle& handle){
            components.emplace_back(Component{
                .owner = 0,
                .resourceHandle = handle
            });
        }

        std::vector<std::reference_wrapper<const Component>> getAllVisible() const{
            std::vector<std::reference_wrapper<const Component>> wrapped;

            std::for_each(components.cbegin(), components.cend(),
                [&wrapped](const auto& comp){
                    wrapped.emplace_back(std::cref(comp));
                }
            );

            return wrapped;
        }
    };
} // namespace ModernBoy

#endif // __INC_RESOURCE_SYSTEM_HPP