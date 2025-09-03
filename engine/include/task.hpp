#ifndef MODERNBOY_TASK_HPP
#define MODERNBOY_TASK_HPP

#include "fwd.hpp"
#include "script/type.hpp"

namespace ModernBoy
{
    struct ActionTask{
        EntityID actor;
        ModuleHandle handle;
        Script::FunctionID function;
    };
} // namespace ModernBoy::Render

#endif // MODERNBOY_TASK_HPP