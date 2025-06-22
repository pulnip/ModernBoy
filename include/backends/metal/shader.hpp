#ifndef MODERNBOY_METAL_SHADER_HPP
#define MODERNBOY_METAL_SHADER_HPP

#include "type.hpp"
#include "fwd.hpp"

namespace ModernBoy::Metal
{
    using ShaderPtr = NativePtr;

    struct Shader{
        ShaderPtr shaderPtr;

        Shader()=default;
        Shader(const Shader&)=delete;
        Shader(Shader&& mesh);
        Shader& operator=(const Shader&)=delete;
        Shader& operator=(Shader&&);

        Shader(ShaderPtr shaderPtr, UI* gui=nullptr);
        ~Shader();

    private:
        // Move semantics
        void moveFrom(Shader&&);

        UI* gui = nullptr;
        ObserverID rp_id = -1;
        ObserverID rs_id = -1;
    };

    #ifdef __cplusplus
extern "C"{
#endif

    extern void* createShader(const void* layerPtr);
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
}

#endif // MODERNBOY_METAL_SHADER_HPP