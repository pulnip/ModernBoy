#ifndef MODERNBOY_SCRIPT_TYPE_HPP
#define MODERNBOY_SCRIPT_TYPE_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "fwd.hpp"
#include "common/type.hpp"
#include "common/alias.hpp"

class asIScriptEngine;
class asIScriptModule;
class asIScriptObject;
class asITypeInfo;

namespace ModernBoy::Script
{
    using FuncName = std::string;
    using FunctionID = ID;

    struct Action{
        ModuleHandle moduleHandle;
        FunctionID function;
    };
    struct Module{
        asIScriptModule* module_ = nullptr;

        Module()=default;
        Module(const Module&)=delete;
        Module(Module&& mesh);
        Module& operator=(const Module&)=delete;
        Module& operator=(Module&&);

        Module(const std::string& name,
            const std::vector<std::string>& files,
            asIScriptEngine* engine
        );

    private:
        // Move semantics
        void moveFrom(Module&&);
    };

    struct Object{
        asITypeInfo* type = nullptr;
        asIScriptObject* object = nullptr;

        Object()=default;
        ~Object();
        Object(const Object&)=delete;
        Object(Object&& mesh);
        Object& operator=(const Object&)=delete;
        Object& operator=(Object&&);

        Object(const std::string& instanceName,
            const std::string& typeName,
            asIScriptModule* module_,
            asIScriptEngine* engine
        );

    private:
        // Move semantics
        void moveFrom(Object&&);
    };
}

#endif // MODERNBOY_SCRIPT_TYPE_HPP