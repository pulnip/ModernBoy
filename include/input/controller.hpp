#ifndef MODERNBOY_INPUT_CONTROLLER_HPP
#define MODERNBOY_INPUT_CONTROLLER_HPP

#include <span>
#include <angelscript.h>
#include "resource_handle.hpp"
#include "resource_manager.hpp"
#include "task_manager.hpp"
#include "input/state.hpp"
#include "input/device.hpp"

namespace ModernBoy{ namespace Input{
    class Controller{
    private:
        using InputTaskManager = TaskManager<InputTask>;
    
    public:
        Controller(Device& device,
            InputTaskManager& taskManager,
            TransformManager& transformManager);
        ~Controller();
        Controller(const Controller& other)=delete;
        Controller(Controller&&)=delete;
        Controller& operator=(const Controller& other)=delete;
        Controller& operator=(Controller&&)=delete;

        void loadModule(std::span<std::string> sourceFiles);
        void update();
    
    private:
        State inputState;

        Device& device;
        InputTaskManager& taskManager;
        TransformManager& transformManager;

        // TODO: add manager for multiple module?
        asIScriptModule* scriptModule;

        asIScriptEngine* scriptEngine;
        asIScriptContext* scriptContext;
    };
}}

#endif // MODERNBOY_INPUT_CONTROLLER_HPP