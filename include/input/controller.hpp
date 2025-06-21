#ifndef MODERNBOY_INPUT_CONTROLLER_HPP
#define MODERNBOY_INPUT_CONTROLLER_HPP

#include <string>
#include <angelscript.h>
#include "resource_handle.hpp"
#include "resource_manager.hpp"
#include "task_system.hpp"
#include "input/state.hpp"
#include "input/device.hpp"
#include "input/component.hpp"

namespace ModernBoy{ namespace Input{
    class Controller{
    private:
        using InputSystem = TaskSystem<Component, Task>;
    
    public:
        Controller(Device& device,
            const std::string& moduleFileName,
            InputSystem& taskManager,
            TransformManager& transformManager);
        ~Controller();
        Controller(const Controller& other)=delete;
        Controller(Controller&&)=delete;
        Controller& operator=(const Controller& other)=delete;
        Controller& operator=(Controller&&)=delete;

        void update();

    private:
        State state;

        Device& device;
        InputSystem& inputSystem;
        TransformManager& transformManager;

        asIScriptEngine* scriptEngine;
        asIScriptContext* scriptContext;
        // TODO: add manager for multiple module?
        asIScriptModule* scriptModule;
    };
}}

#endif // MODERNBOY_INPUT_CONTROLLER_HPP