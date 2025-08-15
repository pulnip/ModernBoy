#ifndef MODERNBOY_SCRIPT_AS_TYPEHELPER_HPP
#define MODERNBOY_SCRIPT_AS_TYPEHELPER_HPP

#include "engine/fwd.hpp"

class asIScriptEngine;

namespace ModernBoy::Script
{
    class TypeHelper{
    public:
        TypeHelper(asIScriptEngine* scriptEngine,
            Engine& engine);

        int registerAll();

    private:
        int registerBasicTypes();
        int registerKeyevent();
        int registerActor();
        int registerComponent();
        int registerGlobalProperty();

    private:
        asIScriptEngine* scriptEngine;
        Engine& engine;
    };
} // namespace ModernBoy::Script

#endif // MODERNBOY_SCRIPT_AS_TYPEHELPER_HPP