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
    other.module_ = nullptr;
}

Module::Module(const std::string& name,
    const std::vector<std::string>& files,
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
    module_ = engine->GetModule(name.c_str());
}

Object::Object(Object&& other){
    moveFrom(std::move(other));
}
Object::~Object(){
    if(object != nullptr)
        object->Release();
}
Object& Object::operator=(Object&& other){
    moveFrom(std::move(other));
    return *this;
}
void Object::moveFrom(Object&& other){
    type = other.type;
    object = other.object;
    other.type = nullptr;
    other.object = nullptr;
}

Object::Object(const std::string& typeName,
    asIScriptModule* module_,
    asIScriptEngine* engine
){
    type = module_->GetTypeInfoByName(typeName.c_str());
    if(type==nullptr)
        throw std::format("Type {} Not Exist!", typeName);
    auto* factory = type->GetFactoryByIndex(0);

    auto* ctx = engine->CreateContext();
    ctx->Prepare(factory);
    ctx->Execute();

    object = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
    object->AddRef();

    ctx->Release();
}
