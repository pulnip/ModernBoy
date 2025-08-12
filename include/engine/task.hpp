#ifndef MODERNBOY_TASK_HPP
#define MODERNBOY_TASK_HPP

#include "engine/fwd.hpp"
#include "engine/script/type.hpp"

namespace ModernBoy
{
    struct ActionTask{
        EntityID actor;
        ModuleHandle handle;
        Script::FunctionID function;
    };
} // namespace ModernBoy::Render

#endif // MODERNBOY_TASK_HPP