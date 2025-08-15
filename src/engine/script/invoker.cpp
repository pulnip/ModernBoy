#include <fstream>
#include <limits>
#include <print>
#include <regex>
#include <sstream>
#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>
#include "engine/engine.hpp"
#include "engine/task.hpp"
#include "engine/script/invoker.hpp"
#include "engine/script/as_helper.hpp"
#include "engine/script/as_stream.hpp"
#include "engine/script/as_typehelper.hpp"
#include "engine/script/type.hpp"
#include "../../../src/game/context.hpp"
#include "../../../src/game/component.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Script;

static void messageCallback(const asSMessageInfo *msg, void *param);
static void print(const std::string& in){
    std::println("{}", in);
}
static void printFloat(float f){
    std::println("{}", f);
}
static void printInt(int i){
    std::println("{}", i);
}

Invoker::Invoker(Game::EntityRegistry& registry,
    ModuleManager& moduleManager, ObjectManager& objectManager,
    Engine& engine
):moduleManager(moduleManager), objectManager(objectManager),
registry(registry), scriptEngine(asCreateScriptEngine()),
scriptContext(scriptEngine->CreateContext()){
    int r = scriptEngine->SetMessageCallback(asFUNCTION(messageCallback), 0, asCALL_CDECL);
    assert(r >= 0);
    RegisterStdString(scriptEngine);

    r = scriptEngine->RegisterGlobalFunction("void print(const string &in)",
        asFUNCTION(print), asCALL_CDECL); assert( r >= 0 );
    r = scriptEngine->RegisterGlobalFunction("void printFloat(float)",
        asFUNCTION(printFloat), asCALL_CDECL); assert( r >= 0 );
    r = scriptEngine->RegisterGlobalFunction("void printInt(int)",
        asFUNCTION(printInt), asCALL_CDECL); assert( r >= 0 );

    TypeHelper typeHelper(scriptEngine, engine);
    typeHelper.registerAll();
}
Invoker::~Invoker(){
    scriptContext->Release();
    scriptEngine->ShutDownAndRelease();
}

FunctionID Invoker::registerFunction(const FuncName& funcName){
    auto it = funcNameToID.find(funcName);
    if(it != funcNameToID.end()){
        return it->second;
    }

    auto newID = issueID();
    auto [_1, ret1] = functionMap.try_emplace(newID, funcName);
    auto [_2, ret2] = funcNameToID.try_emplace(funcName, newID);

    if((!ret1) || (!ret2))
        return std::numeric_limits<FunctionID>::max();
    return newID;
}

ABNORMAL_FLAG Invoker::invoke(ModuleHandle handle,
    FunctionID func_id, EntityID id, DeltaTime deltaTime
){
    auto module_ = moduleManager.get(handle).module_;
    auto funcName = functionMap.at(func_id);

    auto* func = module_->GetFunctionByName(funcName.c_str());
    if(func == nullptr){
        std::println("No function Name {} exists!", funcName);
        return true;
    }

    scriptContext->Prepare(func);

    auto entity = registry.query(id);
    auto dt = deltaTime.count() / 1'000'000.0f;

    scriptContext->SetArgObject(0, &entity);
    scriptContext->SetArgFloat(1, dt);

    auto ret = scriptContext->Execute();
    if(ret != asEXECUTION_FINISHED){
        if(ret == asEXECUTION_EXCEPTION)
            std::println("Exception: {} occured",
                scriptContext->GetExceptionString());
        return true;
    }
    return false;
}

ABNORMAL_FLAG Invoker::invoke(ObjectHandle handle,
    const std::string& funcName, EntityID id, DeltaTime deltaTime
){
    auto& object = objectManager[handle];
    auto* func = object.type->GetMethodByName(funcName.c_str());

    if(func == nullptr){
        std::println("No function Name {} exists!", funcName);
        return true;
    }

    scriptContext->Prepare(func);
    scriptContext->SetObject(object.object);

    auto entity = registry.query(id);
    auto dt = deltaTime.count() / 1'000'000.0f;

    scriptContext->SetArgObject(0, &entity);
    scriptContext->SetArgFloat(1, dt);

    auto ret = scriptContext->Execute();
    if(ret != asEXECUTION_FINISHED){
        if(ret == asEXECUTION_EXCEPTION)
            std::println("Exception: {} occured",
                scriptContext->GetExceptionString());
        return true;
    }
    return false;
}

FunctionID Invoker::issueID(){ return id_seed++; }

static void messageCallback(const asSMessageInfo* msg,
    [[maybe_unused]] void* param
){
    const char *type = "ERR ";
    if(msg->type == asMSGTYPE_WARNING)
        type = "WARN";
    else if( msg->type == asMSGTYPE_INFORMATION)
        type = "INFO";
    std::println("[{}]{}:{}:{}: {}", type,
        msg->section, msg->row, msg->col,
        msg->message);
}
