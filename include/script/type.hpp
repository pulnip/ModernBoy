#ifndef MODERNBOY_SCRIPT_TYPE_HPP
#define MODERNBOY_SCRIPT_TYPE_HPP

#include <cstdint>
#include <string>
#include "fwd.hpp"
#include "common/type.hpp"

namespace ModernBoy::Script
{
    using FuncName = std::string;

    struct Action{
        ModuleHandle moduleHandle;
        FunctionID function;
    };
}

#endif // MODERNBOY_SCRIPT_TYPE_HPP