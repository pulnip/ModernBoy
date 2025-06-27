#include <fstream>
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

static std::pair<std::string, std::string>
parseModuleFunction(const std::string& text);

ActionID Invoker::loadModuleFunction(
    const std::string& name
){
    auto [moduleName, funcName] = parseModuleFunction(name);

    auto module_ = engine->GetModule(moduleName.c_str(),
        asGM_CREATE_IF_NOT_EXISTS);
    Script::StreamWrapper stream(moduleName);
    module_->LoadByteCode(&stream);
    

    return 0;
}

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

void Invoker::buildModule(const ModuleName& moduleName,
    const FileNames& fileNames, const FuncNames& funcNames
){
    CScriptBuilder builder;
    if(builder.StartNewModule(engine, moduleName.c_str()) < 0){
        // If the code fails here it is usually because there
        // is no more memory to allocate the module
        puts("Unrecoverable error while starting a new module.");
        return;
    }
    for(const auto& fileName: fileNames){
        if(builder.AddSectionFromFile(fileName.c_str()) < 0){
            // The builder wasn't able to load the file. Maybe the file
            // has been removed, or the wrong name was given, or some
            // preprocessing commands are incorrectly written.
            puts("Please correct the errors in the script and try again.");
            continue;
        }
    }
    if(builder.BuildModule() < 0){
        // An error occurred. Instruct the script writer to fix the 
        // compilation errors that were listed in the output stream.
        puts("Please correct the errors in the script and try again.\n");
        return;
    }

    asIScriptModule *mod = engine->GetModule(moduleName.c_str());
    for(const auto& funcName: funcNames){
        asIScriptFunction *func = mod->GetFunctionByDecl("void printHello()");
        if(func == 0){
            // The function couldn't be found. Instruct the script writer
            // to include the expected function in the script.
            puts("The script must have the function 'void foo()'. Please add it and try again.");
            return;
        }
        // Create our context, prepare it, and then execute
        context->Prepare(func);
        int r = context->Execute();
        if(r != asEXECUTION_FINISHED){
            // The execution didn't complete as expected. Determine what happened.
            if(r == asEXECUTION_EXCEPTION){
                // An exception occurred, let the script writer know what happened so it can be corrected.
                printf("An exception '%s' occurred. Please correct the code and try again.\n", context->GetExceptionString());
            }
        }

    }
}
