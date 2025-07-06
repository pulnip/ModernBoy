#ifndef MODERNBOY_SCRIPT_INVOKER_HPP
#define MODERNBOY_SCRIPT_INVOKER_HPP

#include <string>
#include <unordered_map>
#include "fwd.hpp"
#include "interface.hpp"
#include "script/type.hpp"
#include "common/alias.hpp"
#include "input/state.hpp"
#include "task.hpp"

class asIScriptContext;

namespace ModernBoy::Script
{
    using FunctionMap = std::unordered_map<
        FunctionID, FuncName>;

    class Invoker{
    public:
        Invoker(AppState& app);
        ~Invoker();
        Invoker(const Invoker& other)=delete;
        Invoker(Invoker&&)=delete;
        Invoker& operator=(const Invoker& other)=delete;
        Invoker& operator=(Invoker&&)=delete;

        FunctionID registerFunction(const FuncName& funcName);
        ABNORMAL_FLAG invokeInput(const Module&, FunctionID, EntityID, Input::Trigger);
        ABNORMAL_FLAG invoke(const Module&, FunctionID, EntityID);

    private:
        FunctionID issueID();

        AppState& app;

        asIScriptEngine* engine;
        asIScriptContext* context;

        FunctionID id_seed = 0;
        FunctionMap functionMap;

        std::vector<ActionTask> tasks;

        friend class ::ModernBoy::AppState;
    };
} // namespace ModernBoy::Input

#endif // MODERNBOY_SCRIPT_INVOKER_HPP