#include <fstream>
#include <sstream>
#include "input/controller.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Input;

Controller::Controller(Device& device,
    InputTaskManager& taskManager,
    TransformManager& transformManager)
:device(device), taskManager(taskManager),
transformManager(transformManager),
scriptEngine(asCreateScriptEngine()),
scriptContext(scriptEngine->CreateContext()){
    scriptEngine->RegisterObjectType("Transform",
        sizeof(Transform), asOBJ_VALUE | asOBJ_POD);
}
Controller::~Controller(){
    scriptContext->Release();
    scriptEngine->ShutDownAndRelease();
}

static std::string load(const std::string& fileName);

void Controller::loadModule(std::span<std::string> sourceFiles){
    scriptModule = scriptEngine->GetModule(
        "game", asGM_ALWAYS_CREATE);
    for(const auto& fileName: sourceFiles){
        auto sourceCode = load(fileName);
        scriptModule->AddScriptSection(
            fileName.c_str(), sourceCode.c_str());
    }
    scriptModule->Build();
}

static std::string load(const std::string& fileName){
    std::ifstream file(fileName);
    if(!file) return "";

    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

void Controller::update(){
    inputState = device.fetch();

    auto inputTasks = taskManager.getAll();
    for(const auto& task: inputTasks){
        auto transform = transformManager.get(
            task.get().getTransformHandle());

        auto func = scriptModule->GetFunctionByName(
            task.get().script.c_str());
        scriptContext->Prepare(func);
        scriptContext->SetArgObject(0, transform);
        scriptContext->Execute();
    }
}
