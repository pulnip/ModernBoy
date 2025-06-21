#ifndef MODERNBOY_INPUT_COMPONENT_HPP
#define MODERNBOY_INPUT_COMPONENT_HPP

#include <functional>
#include <unordered_map>
#include <vector>
#include "raw_resource.hpp"
#include "resource_handle.hpp"
#include "input/state.hpp"

namespace ModernBoy{ namespace Input{
    struct Task{
        Button button;
        ButtonState condition;
        std::string behaviour;

        TransformHandle transformHandle;
    };
    using Tasks = std::vector<Task>;

    using ButtonMap = std::unordered_map<ButtonState, std::string>;
    using InputMap = std::unordered_map<Button, ButtonMap>;

    struct Component{
        EntityID actor;
        InputMap map;

        TransformHandle transformHandle;

        Component(EntityID actor,
            const std::vector<Task>& tasks,
            TransformHandle transformHandle);
        Tasks getTasks() const;
    };

}}

#endif // MODERNBOY_INPUT_COMPONENT_HPP
