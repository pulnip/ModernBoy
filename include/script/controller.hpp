#ifndef MODERNBOY_SCRIPT_CONTROLLER_HPP
#define MODERNBOY_SCRIPT_CONTROLLER_HPP

#include <string>
#include "fwd.hpp"

namespace ModernBoy::Script
{
    class Controller{

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

        class asIScriptEngine* scriptEngine;
        class asIScriptContext* scriptContext;
        // TODO: add manager for multiple module?
        class asIScriptModule* scriptModule;
    };
} // namespace ModernBoy::Script

#endif // MODERNBOY_SCRIPT_CONTROLLER_HPP