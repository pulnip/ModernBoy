#include <fstream>
#include <sstream>
#include <angelscript.h>
#include "app_state.hpp"
#include "task.hpp"
#include "script/controller.hpp"
#include "script/as_helper.hpp"
#include "script/as_stream.hpp"
#include "script/as_typehelper.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Script;

Controller::Controller(AppState& app):app(app),
scriptEngine(asCreateScriptEngine()),
scriptContext(scriptEngine->CreateContext()
){
    Script::registerTransform(scriptEngine);
}
Controller::~Controller(){
    scriptContext->Release();
    scriptEngine->ShutDownAndRelease();
}

void Controller::loadScriptModule(const std::string& moduleFileName){
    scriptModule = scriptEngine->GetModule(
        "ModernBoy", asGM_ALWAYS_CREATE);
    Script::StreamWrapper stream(moduleFileName);
    scriptModule->LoadByteCode(&stream);
}

void Controller::update(){
    if(scriptModule == nullptr)
        return;
    // ToDo. InputSystem might be handle it.
    // auto tasks = app.get<InputTask>(state);

    // for(auto& task: tasks){
    //     auto func = scriptModule->GetFunctionByName(
    //         task.behaviour.c_str());
    //     scriptContext->Prepare(func);
    //     scriptContext->SetArgObject(0, &(task.transform));
    //     if(scriptContext->Execute() < 0)
    //         Script::printExceptionInfo(scriptContext);
    // }
}
