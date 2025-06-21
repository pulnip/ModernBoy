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
    InputTaskManager& taskManager,
    TransformManager& transformManager)
:device(device), taskManager(taskManager),
transformManager(transformManager),
scriptEngine(asCreateScriptEngine()),
scriptContext(scriptEngine->CreateContext()){
    Util::registerTransform(scriptEngine);

    Util::StreamWrapper stream(moduleFileName);
    scriptModule->SaveByteCode(&stream);
}
Controller::~Controller(){
    scriptContext->Release();
    scriptEngine->ShutDownAndRelease();
}

void Controller::update(){
    device.fetch(state);

    auto inputTasks = taskManager.getAll();
    for(const auto& task: inputTasks){
        auto transform = transformManager.get(
            task.get().getTransformHandle());

        auto func = scriptModule->GetFunctionByName(
            task.get().script.c_str());
        scriptContext->Prepare(func);
        scriptContext->SetArgObject(0, transform);
        if(auto ret = scriptContext->Execute() < 0)
            Util::printExceptionInfo(scriptContext);
    }
}
