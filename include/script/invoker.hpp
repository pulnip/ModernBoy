#ifndef MODERNBOY_SCRIPT_INVOKER_HPP
#define MODERNBOY_SCRIPT_INVOKER_HPP

#include <string>
#include <unordered_map>
#include "fwd.hpp"

namespace ModernBoy::Script
{
    using ModuleName = std::string;
    using FileName = std::string;
    using FileNames = std::vector<FileName>;
    using FuncName = std::string;
    using FuncNames = std::vector<FuncName>;
    using FileMap = std::unordered_map<
        FileName, FuncName>;

    using ModuleID = uint16_t;
    using FunctionID = uint16_t;

    class Invoker{

    public:
        Invoker(AppState& app);
        ~Invoker();
        Invoker(const Invoker& other)=delete;
        Invoker(Invoker&&)=delete;
        Invoker& operator=(const Invoker& other)=delete;
        Invoker& operator=(Invoker&&)=delete;

        ActionID loadModuleFunction(const std::string& name);
        void buildModule(const ModuleName& moduleName,
            const FileNames& fileNames, const FuncNames& funcNames);

    private:
        AppState& app;

        class asIScriptEngine* engine;
        class asIScriptContext* context;

        ModuleID id_seed = 0;

        using ModuleMap = std::unordered_map<
            ModuleID, FunctionID*>;
        ModuleMap moduleMap;
    };
} // namespace ModernBoy::Input

#endif // MODERNBOY_SCRIPT_INVOKER_HPP