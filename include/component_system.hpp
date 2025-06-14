#ifndef MODERNBOY_COMPONENT_SYSTEM_HPP
#define MODERNBOY_COMPONENT_SYSTEM_HPP

#include <algorithm>
#include <functional>
#include <vector>

namespace ModernBoy
{
    template<typename Component>
    class ComponentSystem{
    private:
        std::vector<Component> components;

    public:
        template<typename... Args>
        void create(Args... args){
            components.emplace_back(Component(args...));
        }

        auto getAll() const{
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

#endif // MODERNBOY_COMPONENT_SYSTEM_HPP