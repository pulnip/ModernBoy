#include <string>
#include <utility>
#include <vector>
#include "backends/metal/texture.hpp"
#include "app_state.hpp"

#ifdef __cplusplus
extern "C"{
#endif

    extern void* createTexture(const char* filePath,
        const void* layerPtr);
    extern void destroyTexture(const void* texturePtr);

#ifdef __cplusplus
}
#endif

using namespace ModernBoy;
using namespace ModernBoy::Metal;

Texture::Texture(Texture&& other){ moveFrom(std::move(other)); }
Texture& Texture::operator=(Texture&& other){
    moveFrom(std::move(other));
    return *this;
}
void Texture::moveFrom(Texture&& other){
    texture = other.texture;
    other.texture = nullptr;
}
Texture::~Texture(){
    if(texture != nullptr)
        destroyTexture(texture);
}

Texture::Texture(const std::string& fileName,
            NativePtr layerPtr)
:texture(createTexture(fileName.c_str(), layerPtr)){}
