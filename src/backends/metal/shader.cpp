#include <utility>
#include "backends/metal/shader.hpp"

using namespace ModernBoy::Metal;

Shader::Shader(Shader&& other){ moveFrom(std::move(other)); }
Shader& Shader::operator=(Shader&& other){
    moveFrom(std::move(other));
    return *this;
}
void Shader::moveFrom(Shader&& other){
    shaderPtr = other.shaderPtr;
    other.shaderPtr = nullptr;
}
