#include <fstream>
#include <sstream>
#include "input/controller.hpp"
#include "util/as_helper.hpp"
#include "util/as_stream.hpp"
#include "util/as_typehelper.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Input;

Controller::Controller(Device& device,
    const std::string& moduleFileName,
    InputSystem& inputSystem,
    TransformManager& transformManager)
:device(device), inputSystem(inputSystem),
transformManager(transformManager),
scriptEngine(asCreateScriptEngine()),
scriptContext(scriptEngine->CreateContext()),
scriptModule(scriptModule = scriptEngine->GetModule(
    "ModernBoy", asGM_ALWAYS_CREATE)
){
    Util::registerTransform(scriptEngine);

    Util::StreamWrapper stream(moduleFileName);
    scriptModule->LoadByteCode(&stream);
}
Controller::~Controller(){
    scriptContext->Release();
    scriptEngine->ShutDownAndRelease();
}

void Controller::update(){
    device.fetch(state);
    
    auto inputTasks = inputSystem.getAll();
    std::erase_if(inputTasks,
        [&state=(this->state)](const Task& task){
            return task.condition != state.key[task.button];
        }
    );
    for(const auto& task: inputTasks){
        auto transform = transformManager.get(
            task.transformHandle);

        auto func = scriptModule->GetFunctionByName(
            task.behaviour.c_str());
        scriptContext->Prepare(func);
        scriptContext->SetArgObject(0, transform);
        if(scriptContext->Execute() < 0)
            Util::printExceptionInfo(scriptContext);
    }
}
