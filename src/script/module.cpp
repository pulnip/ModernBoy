#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>
#include "script/type.hpp"

using namespace ModernBoy::Script;

Module::Module(Module&& other){
    moveFrom(std::move(other));
}
Module& Module::operator=(Module&& other){
    moveFrom(std::move(other));
    return *this;
}
void Module::moveFrom(Module&& other){
    module_ = other.module_;
    name = std::move(other.name);
    other.module_ = nullptr;
    other.name.clear();
}

Module::Module(const std::string& name,
    std::span<std::string> files,
    asIScriptEngine* engine
){
    CScriptBuilder builder;
    if(builder.StartNewModule(engine, name.c_str()) < 0){
        // If the code fails here it is usually because there
        // is no more memory to allocate the module
        throw "Unrecoverable error while starting a new module.";
    }
    for(const auto& file: files){
        if(builder.AddSectionFromFile(file.c_str()) < 0){
            // The builder wasn't able to load the file. Maybe the file
            // has been removed, or the wrong name was given, or some
            // preprocessing commands are incorrectly written.
            throw "Please correct the errors in the script and try again.";
        }
    }
    if(builder.BuildModule() < 0){
        // An error occurred. Instruct the script writer to fix the 
        // compilation errors that were listed in the output stream.
        throw "Please correct the errors in the script and try again.";
    }
}

void Module::bind(asIScriptContext* context,
    const std::string& funcName
){
    auto func = module_->GetFunctionByName(funcName.c_str());
    if(func == 0){
        // The function couldn't be found. Instruct the script writer
        // to include the expected function in the script.
        throw "The script must have the function 'void foo()'. Please add it and try again.";
    }
    context->Prepare(func);
}

