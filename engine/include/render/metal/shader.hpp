#ifndef MODERNBOY_METAL_SHADER_HPP
#define MODERNBOY_METAL_SHADER_HPP

#include "fwd.hpp"

namespace ModernBoy::Metal
{
    using ShaderPtr = NativePtr;

    struct Shader{
        ShaderPtr shaderPtr;

        Shader(NativePtr renderContext,
            const std::string& vsFuncName = "vertex_main",
            const std::string& fsFuncName = "fragment_main",
            bool useDepth = true);
        ~Shader();

        Shader()=default;
        Shader(const Shader&)=delete;
        Shader(Shader&& mesh);
        Shader& operator=(const Shader&)=delete;
        Shader& operator=(Shader&&);

    private:
        // Move semantics
        void moveFrom(Shader&&);
    };
}

#endif // MODERNBOY_METAL_SHADER_HPP