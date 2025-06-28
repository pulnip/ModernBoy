#ifndef MODERNBOY_METAL_SHADER_HPP
#define MODERNBOY_METAL_SHADER_HPP

#include "fwd.hpp"

namespace ModernBoy::Metal
{
    using ShaderPtr = NativePtr;

    struct Shader{
        ShaderPtr shaderPtr;

        Shader(const std::string& fileName,
            NativePtr layerPtr, UI* gui=nullptr);
        ~Shader();

        Shader()=default;
        Shader(const Shader&)=delete;
        Shader(Shader&& mesh);
        Shader& operator=(const Shader&)=delete;
        Shader& operator=(Shader&&);

    private:
        // Move semantics
        void moveFrom(Shader&&);

        UI* gui = nullptr;
        ObserverID rp_id = -1;
        ObserverID rs_id = -1;
    };
}

#endif // MODERNBOY_METAL_SHADER_HPP