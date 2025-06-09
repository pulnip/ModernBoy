#ifndef MODERNBOY_METAL_SHADER_HPP
#define MODERNBOY_METAL_SHADER_HPP

#include "type.hpp"

namespace ModernBoy::Metal
{
#ifdef __cplusplus
extern "C"{
#endif

    extern void* createShader(const void* layerPtr);
    extern void destroyShader(const void* shaderPtr);

#ifdef __cplusplus
}
#endif
    using ShaderPtr = NativePtr;

        struct Shader{
        ShaderPtr shaderPtr;

        Shader()=default;
        Shader(const Shader&)=delete;
        Shader(Shader&& mesh);
        Shader& operator=(const Shader&)=delete;
        Shader& operator=(Shader&&);

        Shader(ShaderPtr shaderPtr):shaderPtr(shaderPtr){}
        ~Shader(){ destroyShader(shaderPtr); }

    private:
        // Move semantics
        void moveFrom(Shader&&);
    };
}

#endif // MODERNBOY_METAL_SHADER_HPP