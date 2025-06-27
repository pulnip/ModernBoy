#ifndef MODERNBOY_SCRIPT_TYPE_HPP
#define MODERNBOY_SCRIPT_TYPE_HPP

#include <span>
#include <string>

namespace ModernBoy::Script
{
    struct Module{
        std::string name;
        class asIScriptModule* module_;

        Module()=default;
        Module(const Module&)=delete;
        Module(Module&& mesh);
        Module& operator=(const Module&)=delete;
        Module& operator=(Module&&);

        Module(const std::string& name,
            std::span<std::string> files,
            class asIScriptEngine* engine
        );

        void bind(class asIScriptContext* context,
            const std::string& funcName);

    private:
        // Move semantics
        void moveFrom(Module&&);
    };

    struct Context{
        
    };
} // namespace ModernBoy::Script

#endif // MODERNBOY_SCRIPT_TYPE_HPP