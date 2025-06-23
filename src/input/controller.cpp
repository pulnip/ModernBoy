#include <fstream>
#include <sstream>
#include <angelscript.h>
#include "app_state.hpp"
#include "task.hpp"
#include "input/controller.hpp"
#include "util/as_helper.hpp"
#include "util/as_stream.hpp"
#include "util/as_typehelper.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Input;

constexpr auto KB_IN_BIT = bit_of<InputComponent, TransformComponent>();

Controller::Controller(AppState& app):app(app),
scriptEngine(asCreateScriptEngine()),
scriptContext(scriptEngine->CreateContext()
){
    Util::registerTransform(scriptEngine);
}
Controller::~Controller(){
    scriptContext->Release();
    scriptEngine->ShutDownAndRelease();
}

void Controller::loadScriptModule(const std::string& moduleFileName){
    scriptModule = scriptEngine->GetModule(
        "ModernBoy", asGM_ALWAYS_CREATE);
    Util::StreamWrapper stream(moduleFileName);
    scriptModule->LoadByteCode(&stream);
}

void Controller::update(){
    if(scriptModule == nullptr)
        return;
    app.inputDevice.fetch(state);
    auto tasks = app.get<InputTask>(state);

    for(auto& task: tasks){
        auto func = scriptModule->GetFunctionByName(
            task.behaviour.c_str());
        scriptContext->Prepare(func);
        scriptContext->SetArgObject(0, &(task.transform));
        if(scriptContext->Execute() < 0)
            Util::printExceptionInfo(scriptContext);
    }
}
