#ifndef MODERNBOY_SCRIPT_INVOKER_HPP
#define MODERNBOY_SCRIPT_INVOKER_HPP

#include <string>
#include <unordered_map>
#include "interface.hpp"
#include "common/alias.hpp"
#include "engine/script/type.hpp"
#include "engine/input/state.hpp"
#include "task.hpp"
#include "game/game_fwd.hpp"

class asIScriptContext;

namespace ModernBoy::Script
{
    using FunctionMap = std::unordered_map<
        FunctionID, FuncName>;

    class Invoker{
    public:
        Invoker(Game::EntityRegistry& registry,
            ModuleManager& moduleManager, ObjectManager& objectManager,
            Input::Device& device);
        ~Invoker();
        Invoker(const Invoker& other)=delete;
        Invoker(Invoker&&)=delete;
        Invoker& operator=(const Invoker& other)=delete;
        Invoker& operator=(Invoker&&)=delete;

        FunctionID registerFunction(const FuncName& funcName);
        ABNORMAL_FLAG invoke(ModuleHandle, FunctionID,
            EntityID, DeltaTime);
        ABNORMAL_FLAG invoke(ObjectHandle, const std::string& func,
            EntityID, DeltaTime);

    private:
        FunctionID issueID();

        ModuleManager& moduleManager;
        ObjectManager& objectManager;
        Game::EntityRegistry& registry;

        asIScriptEngine* engine;
        asIScriptContext* context;

        FunctionID id_seed = 0;
        std::unordered_map<FunctionID, FuncName> functionMap;
        std::unordered_map<FuncName, FunctionID> funcNameToID;

        std::vector<ActionTask> tasks;

        friend class ::ModernBoy::AppState;
    };
} // namespace ModernBoy::Input

#endif // MODERNBOY_SCRIPT_INVOKER_HPP