#ifndef MODERNBOY_SCRIPT_AS_TYPEHELPER_HPP
#define MODERNBOY_SCRIPT_AS_TYPEHELPER_HPP

#include "fwd.hpp"

class asIScriptEngine;

namespace ModernBoy::Script
{
    int registerTransform(asIScriptEngine* engine);
    int registerKeyevent(asIScriptEngine* engine);
    int registerActor(asIScriptEngine* engine);

} // namespace ModernBoy::Script

#endif // MODERNBOY_SCRIPT_AS_TYPEHELPER_HPP