#ifndef MODERNBOY_INPUT_CONTROLLER_HPP
#define MODERNBOY_INPUT_CONTROLLER_HPP

#include <string>
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
            const std::string& moduleFileName,
            InputTaskManager& taskManager,
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
        InputTaskManager& taskManager;
        TransformManager& transformManager;

        // TODO: add manager for multiple module?
        asIScriptModule* scriptModule;

        asIScriptEngine* scriptEngine;
        asIScriptContext* scriptContext;
    };
}}

#endif // MODERNBOY_INPUT_CONTROLLER_HPP