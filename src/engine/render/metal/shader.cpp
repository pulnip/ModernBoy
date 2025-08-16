#include <utility>
#include "engine/render/metal/shader.hpp"

extern "C"{
    extern void* createShader(void* rctxPtr,
        const char* vsFuncName, const char* fsFuncName,
        bool useDepth);
    extern void destroyShader(const void* shaderPtr);
}

using namespace ModernBoy::Metal;

Shader::Shader(NativePtr renderContext,
    const std::string& vsFuncName,
    const std::string& fsFuncName,
    bool useDepth)
:shaderPtr(createShader(renderContext,
    vsFuncName.c_str(), fsFuncName.c_str(), useDepth)){}
Shader::~Shader(){
    if(shaderPtr != nullptr){
        destroyShader(shaderPtr);
    }
}
Shader::Shader(Shader&& other){ moveFrom(std::move(other)); }
Shader& Shader::operator=(Shader&& other){
    moveFrom(std::move(other));
    return *this;
}
void Shader::moveFrom(Shader&& other){
    shaderPtr = other.shaderPtr;

    other.shaderPtr = nullptr;
}
