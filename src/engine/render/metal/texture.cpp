#include <string>
#include <utility>
#include <vector>
#include "engine/log.hpp"
#include "engine/render/metal/texture.hpp"
#include "engine/engine.hpp"

extern "C"{
    extern void* createTextureFromPath(const void* rctxPtr,
        const char* filePath);
    extern void* createTextureFromPixel(const void* rctxPtr,
        uint8_t* pixels, int32_t width, int32_t height);
    extern void destroyTexture(const void* texturePtr);
}

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
    if(texture != nullptr){
        AppTrace("  Texture {} destroyed", texture);
        destroyTexture(texture);
    }
}

Texture::Texture(NativePtr rctxPtr,
    const std::string& fileName)
:texture(createTextureFromPath(rctxPtr, fileName.c_str())){
    AppTrace("  Texture: Ptr: {}", texture);
}
Texture::Texture(NativePtr rctxPtr,
    uint8_t* pixel, int width, int height)
:texture(createTextureFromPixel(rctxPtr, pixel, width, height)){
    AppTrace("  Texture: Ptr: {}", texture);
}
