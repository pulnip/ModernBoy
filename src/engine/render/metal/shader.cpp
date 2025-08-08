#include <utility>
#include "engine/render/metal/shader.hpp"

#ifdef __cplusplus
extern "C"{
#endif

    extern void* createShader(void* rctxPtr,
        const char* vsFuncName,
        const char* fsFuncName);
    extern void destroyShader(const void* shaderPtr);

    extern void Shader_setRimPower(
        void* shaderPtr, float rimPower
    );
    extern void Shader_setRimStrength(
        void* shaderPtr, float rimStrength
    );

#ifdef __cplusplus
}
#endif

using namespace ModernBoy::Metal;

Shader::Shader(NativePtr renderContext,
    const std::string& vsFuncName,
    const std::string& fsFuncName)
:shaderPtr(createShader(renderContext,
    vsFuncName.c_str(), fsFuncName.c_str())){}
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
