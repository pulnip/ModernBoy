#include <fstream>
#include <limits>
#include <print>
#include <regex>
#include <sstream>
#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>
#include "app_state.hpp"
#include "task.hpp"
#include "script/invoker.hpp"
#include "script/as_helper.hpp"
#include "script/as_stream.hpp"
#include "script/as_typehelper.hpp"
#include "script/type.hpp"
#include "game/context.hpp"
#include "game/component.hpp"

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
    Input::Chord& chord
):moduleManager(moduleManager), objectManager(objectManager),
registry(registry), engine(asCreateScriptEngine()),
context(engine->CreateContext()){
    int r = engine->SetMessageCallback(asFUNCTION(messageCallback), 0, asCALL_CDECL);
    assert(r >= 0);
    RegisterStdString(engine);

    r = engine->RegisterGlobalFunction("void print(const string &in)",
        asFUNCTION(print), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("void printFloat(float)",
        asFUNCTION(printFloat), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("void printInt(int)",
        asFUNCTION(printInt), asCALL_CDECL); assert( r >= 0 );

    TypeHelper typeHelper(engine, chord);
    typeHelper.registerAll();
}
Invoker::~Invoker(){
    context->Release();
    engine->ShutDownAndRelease();
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

ABNORMAL_FLAG Invoker::invokeInput(const Module& module_,
    FunctionID func_id, EntityID id, DeltaTime deltaTime,
    Input::Trigger state
){
    auto mod = module_.module_;
    auto funcName = functionMap.at(func_id);

    auto* func = mod->GetFunctionByName(funcName.c_str());
    if(func == nullptr){
        std::println("No function Name {} exists!", funcName);
        return true;
    }

    if(auto ret = context->Prepare(func)){
        if(ret != 0) return true;
    }

    auto entity = registry.query(id);
    auto dt = deltaTime.count() / 1'000'000.0f;

    context->SetArgObject(0, &entity);
    context->SetArgFloat(1, dt);

    auto ret = context->Execute();
    if(ret != asEXECUTION_FINISHED){
        if(ret == asEXECUTION_EXCEPTION)
            std::println("Exception: {} occured",
                context->GetExceptionString());
        return true;
    }
    return false;
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

    context->Prepare(func);

    auto entity = registry.query(id);
    auto dt = deltaTime.count() / 1'000'000.0f;

    context->SetArgObject(0, &entity);
    context->SetArgFloat(1, dt);

    auto ret = context->Execute();
    if(ret != asEXECUTION_FINISHED){
        if(ret == asEXECUTION_EXCEPTION)
            std::println("Exception: {} occured",
                context->GetExceptionString());
        return true;
    }
    return false;
}

ABNORMAL_FLAG Invoker::invoke(ObjectHandle handle,
    const std::string& funcName, EntityID id, DeltaTime deltaTime
){
    auto& object = objectManager.get(handle);
    auto* func = object.type->GetMethodByName(funcName.c_str());

    if(func == nullptr){
        std::println("No function Name {} exists!", funcName);
        return true;
    }

    context->Prepare(func);
    context->SetObject(object.object);

    auto entity = registry.query(id);
    auto dt = deltaTime.count() / 1'000'000.0f;

    context->SetArgObject(0, &entity);
    context->SetArgFloat(1, dt);

    auto ret = context->Execute();
    if(ret != asEXECUTION_FINISHED){
        if(ret == asEXECUTION_EXCEPTION)
            std::println("Exception: {} occured",
                context->GetExceptionString());
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
