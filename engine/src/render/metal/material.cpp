#include <string>
#include <utility>
#include <vector>
#include "log/log.hpp"
#include "render/metal/material.hpp"
#include "engine.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Metal;

extern "C"{
    extern NativePtr createUnlitMaterialFromPath(
        const NativePtr rctxPtr,
        const char* baseColorFilePath);
    extern NativePtr createUnlitMaterialFromPixel(
        const NativePtr rctxPtr, const uint8_t* pixels,
        int32_t width, int32_t height);
    extern NativePtr createUnlitMaterialFromASTC(
        const NativePtr rctxPtr, GpuPayload*);
    extern void destroyUnlitMaterial(
        NativePtr nativeMaterial);
}

UnlitMaterial::UnlitMaterial(
    NativePtr rctxPtr,
    const std::string& filePath)
:material(createUnlitMaterialFromPath(
    rctxPtr, filePath.c_str())
){
    AppDebug("UnlitMaterial: {}", filePath);
}
UnlitMaterial::UnlitMaterial(
    NativePtr rctxPtr,
    std::span<const uint8_t> pixels,
    int width, int height)
:material(createUnlitMaterialFromPixel(
    rctxPtr, pixels.data(), width, height)
){
    AppTrace("UnlitMaterial: Ptr: {}", material);
}
UnlitMaterial::UnlitMaterial(
    NativePtr rctxPtr,
    GpuPayload* astcPayload)
:material(createUnlitMaterialFromASTC(
    rctxPtr, astcPayload)
){}

UnlitMaterial::~UnlitMaterial(){
    if(material != nullptr){
        AppTrace("UnlitMaterial {} destroyed", material);
        destroyUnlitMaterial(material);
    }
}

UnlitMaterial::UnlitMaterial(UnlitMaterial&& other){
    moveFrom(std::move(other)); }
UnlitMaterial& UnlitMaterial::operator=(UnlitMaterial&& other){
    moveFrom(std::move(other));
    return *this;
}
void UnlitMaterial::moveFrom(UnlitMaterial&& other){
    material = other.material;
    other.material = nullptr;
}
