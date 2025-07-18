#ifndef MODERNBOY_SCRIPT_AS_TYPEHELPER_HPP
#define MODERNBOY_SCRIPT_AS_TYPEHELPER_HPP

#include "fwd.hpp"
#include "game/game_fwd.hpp"

class asIScriptEngine;

namespace ModernBoy::Script
{
    class TypeHelper{
    public:
        TypeHelper(asIScriptEngine* engine, 
            Input::Chord& chord);

        int registerAll();

    private:
        int registerBasicTypes();
        int registerKeyevent();
        int registerActor();
        int registerComponent();
        int registerGlobalProperty();

    private:
        asIScriptEngine* engine;
        Input::Chord& chord;
    };
} // namespace ModernBoy::Script

#endif // MODERNBOY_SCRIPT_AS_TYPEHELPER_HPP