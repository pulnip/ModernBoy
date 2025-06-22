#ifndef MODERNBOY_INPUT_COMPONENT_HPP
#define MODERNBOY_INPUT_COMPONENT_HPP

#include <functional>
#include <unordered_map>
#include <vector>
#include "resource_handle.hpp"
#include "input/state.hpp"

namespace ModernBoy::Input
{
    struct Task{
        Button button;
        ButtonState condition;
        std::string behaviour;

        TransformHandle transformHandle;
    };
    using Tasks = std::vector<Task>;

    using ButtonMap = std::unordered_map<ButtonState, std::string>;
    using InputMap = std::unordered_map<Button, ButtonMap>;

    struct Behaviour{
        Button button;
        ButtonState condition;
        std::string behaviour;
    };
    using Behaviours = std::vector<Behaviour>;

    struct Component{
        EntityID actor;
        InputMap map;

        TransformHandle transformHandle;

        Component(EntityID actor,
            const Behaviours& tasks,
            TransformHandle transformHandle);
        Tasks getTasks() const;
    };
}

#endif // MODERNBOY_INPUT_COMPONENT_HPP
