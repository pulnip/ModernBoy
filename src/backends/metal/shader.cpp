#include <utility>
#include "backends/metal/shader.hpp"

#ifdef __cplusplus
extern "C"{
#endif

    extern void* createShader(const char* filePath,
        const void* layerPtr);
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

Shader::Shader(const std::string& fileName,
    NativePtr layerPtr)
:shaderPtr(createShader(fileName.c_str(), layerPtr)){}
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
