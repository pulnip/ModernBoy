#ifndef MODERNBOY_INPUT_CONTROLLER_HPP
#define MODERNBOY_INPUT_CONTROLLER_HPP

#include <string>
#include "fwd.hpp"
#include "input/state.hpp"

namespace ModernBoy::Input
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

        State state;

        class asIScriptEngine* scriptEngine;
        class asIScriptContext* scriptContext;
        // TODO: add manager for multiple module?
        class asIScriptModule* scriptModule;
    };
} // namespace ModernBoy::Input

#endif // MODERNBOY_INPUT_CONTROLLER_HPP