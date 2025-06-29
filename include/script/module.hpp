#ifndef MODERNBOY_SCRIPT_MODULE_HPP
#define MODERNBOY_SCRIPT_MODULE_HPP

#include <string>
#include <vector>

class asIScriptModule;
class asIScriptEngine;

namespace ModernBoy::Script
{
    struct Module{
        asIScriptModule* module_;

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
} // namespace ModernBoy::Script

#endif // MODERNBOY_SCRIPT_MODULE_HPP