#include <format>
#include <fstream>
#include <print>
#include <sstream>
#include <angelscript.h>
#include <raw_resource.hpp>
#include "script/as_stream.hpp"
#include "script/as_typehelper.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Script;

static std::string load(const std::string& sourceFileName);

static void MessageCallback(
    const asSMessageInfo *msg,
    [[maybe_unused]] void *param
){
    const char *type = "ERR ";
    if( msg->type == asMSGTYPE_WARNING ) 
        type = "WARN";
    else if( msg->type == asMSGTYPE_INFORMATION ) 
        type = "INFO";
    printf("%s(%d:%d): %s: %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

int main(int argc, char* argv[]){
    if(argc < 2){
        puts("No Angelscript Module!");
        return -1;
    }

    auto scriptEngine = asCreateScriptEngine();
    scriptEngine->SetMessageCallback(
        asFUNCTION(MessageCallback), 0, asCALL_CDECL);
    auto scriptModule = scriptEngine->GetModule(argv[1],
        asGM_ALWAYS_CREATE
    );
    if(auto ret = registerTransform(scriptEngine) < 0){
        puts("RegisterTransform failed!");
        return ret;
    }

    for(int i=2; i<argc; ++i){
        auto scriptSource = load(argv[i]);
        if(!scriptSource.length()) continue;
        if(auto ret = scriptModule->AddScriptSection(
            argv[i], scriptSource.c_str()) < 0
        ){
            printf("AddScriptSection Failed: %s\n", argv[i]);
            return ret;
        }
    }
    if(auto ret = scriptModule->Build()){
        puts("Angelscript Build failed!");
        return ret;
    }

    auto outFile = std::format("{}.asb", argv[1]);
    StreamWrapper outStream(outFile, STREAM_WRITE);
    if(auto ret = scriptModule->SaveByteCode(&outStream) < 0){
        puts("SaveByteCode Failed!");
        return ret;
    }

    puts("Angelscript Build Success!");
    scriptEngine->ShutDownAndRelease();
    return 0;
}

static std::string load(const std::string& sourceFileName){
    std::ifstream file(sourceFileName);
    if(!file) return "";

    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}
