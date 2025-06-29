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
#include "script/module.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Script;

static void messageCallback(const asSMessageInfo *msg, void *param);
static void print(const std::string& in){
    std::println("{}", in);
}

Invoker::Invoker(AppState& app):app(app),
engine(asCreateScriptEngine()),
context(engine->CreateContext()){
    int r = engine->SetMessageCallback(asFUNCTION(messageCallback), 0, asCALL_CDECL);
    assert(r >= 0);
    RegisterStdString(engine);

    r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); assert( r >= 0 );

    Script::registerTransform(engine);
}
Invoker::~Invoker(){
    context->Release();
    engine->ShutDownAndRelease();
}

FunctionID Invoker::registerFunction(const FuncName& funcName){
    auto newID = issueID();
    auto [it, ret] = functionMap.try_emplace(newID, funcName);

    if(!ret)
        return std::numeric_limits<FunctionID>::max();
    return newID;
}

ABNORMAL_FLAG Invoker::invoke(const Module& module_, FunctionID id){
    auto mod = module_.module_;
    auto funcName = functionMap.at(id);

    std::println("Invoke {}", id);

    auto* func = mod->GetFunctionByName(funcName.c_str());
    if(func == nullptr){
        std::println("No function Name {} exists!", funcName);
        return true;
    }

    context->Prepare(func);
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

static void messageCallback(const asSMessageInfo *msg, void *param){
    const char *type = "ERR ";
    if(msg->type == asMSGTYPE_WARNING)
        type = "WARN";
    else if( msg->type == asMSGTYPE_INFORMATION)
        type = "INFO";
    std::println("[{}]{}:{}:{}: {}", type,
        msg->section, msg->row, msg->col,
        msg->message);
}

static std::pair<std::string, std::string>
parseModuleFunction(const std::string& text){
        std::regex re(R"(\.)");
    std::sregex_token_iterator iter(text.begin(), text.end(), re, -1);
    std::sregex_token_iterator end;

    std::vector<std::string> result(3);
    for(; iter!=end; ++iter){
        result.push_back(iter->str());
    }

    return {result[0], result[1]};
}
