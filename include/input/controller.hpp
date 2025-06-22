#ifndef MODERNBOY_INPUT_CONTROLLER_HPP
#define MODERNBOY_INPUT_CONTROLLER_HPP

#include <string>
#include <angelscript.h>
#include "fwd.hpp"
#include "task_system.hpp"
#include "input/component.hpp"

namespace ModernBoy::Input
{
    class Controller{
    private:
        using InputSystem = TaskSystem<Component, Task>;
    
    public:
        Controller(AppState& app);
        ~Controller();
        Controller(const Controller& other)=delete;
        Controller(Controller&&)=delete;
        Controller& operator=(const Controller& other)=delete;
        Controller& operator=(Controller&&)=delete;

        void loadScriptModule(const std::string& moduleFileName);
        void update();

    private:
        AppState& app;

        State state;

        asIScriptEngine* scriptEngine;
        asIScriptContext* scriptContext;
        // TODO: add manager for multiple module?
        asIScriptModule* scriptModule;
    };
} // namespace ModernBoy::Input

#endif // MODERNBOY_INPUT_CONTROLLER_HPP