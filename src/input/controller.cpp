#include <fstream>
#include <sstream>
#include <angelscript.h>
#include "game/app_state.hpp"
#include "input/controller.hpp"
#include "input/component.hpp"
#include "util/as_helper.hpp"
#include "util/as_stream.hpp"
#include "util/as_typehelper.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Input;

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
    
    auto inputTasks = app.inputSystem.getAll();
    std::erase_if(inputTasks,
        [&state=(this->state)](const Task& task){
            return task.condition != state.key[task.button];
        }
    );
    for(const auto& task: inputTasks){
        auto transform = app.transformManager.get(
            task.transformHandle);

        auto func = scriptModule->GetFunctionByName(
            task.behaviour.c_str());
        scriptContext->Prepare(func);
        scriptContext->SetArgObject(0, transform);
        if(scriptContext->Execute() < 0)
            Util::printExceptionInfo(scriptContext);
    }
}
