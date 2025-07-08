#ifndef MODERNBOY_SCRIPT_AS_TYPEHELPER_HPP
#define MODERNBOY_SCRIPT_AS_TYPEHELPER_HPP

#include "fwd.hpp"

class asIScriptEngine;

namespace ModernBoy::Script
{
    class TypeHelper{
    public:
        TypeHelper(asIScriptEngine* engine, 
            Input::Chord& chord);

        int registerTransform();
        int registerKeyevent();
        int registerActor();
        int registerGlobalProperty();

    private:
        asIScriptEngine* engine;
        Input::Chord& chord;
    };

} // namespace ModernBoy::Script

#endif // MODERNBOY_SCRIPT_AS_TYPEHELPER_HPP