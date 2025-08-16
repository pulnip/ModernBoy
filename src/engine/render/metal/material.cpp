#include <string>
#include <utility>
#include <vector>
#include "engine/log.hpp"
#include "engine/render/metal/material.hpp"
#include "engine/engine.hpp"

extern "C"{
    extern void* createPBRMaterial(const void* rctxPtr,
        const void* shaderPtr,
        const void* baseColorPtr, const void* normalPtr,
        const void* mrPtr, const void* emissivePtr);
    extern void destroyMaterial(const void* materialPtr);
}

using namespace ModernBoy;
using namespace ModernBoy::Metal;

PBRMaterial::PBRMaterial(PBRMaterial&& other){
    moveFrom(std::move(other)); }
PBRMaterial& PBRMaterial::operator=(PBRMaterial&& other){
    moveFrom(std::move(other));
    return *this;
}
void PBRMaterial::moveFrom(PBRMaterial&& other){
    material = other.material;
    other.material = nullptr;
}
PBRMaterial::~PBRMaterial(){
    if(material != nullptr){
        AppTrace("  PBRMaterial {} destroyed", material);
        destroyMaterial(material);
    }
}

PBRMaterial::PBRMaterial(NativePtr rctxPtr, NativePtr shaderPtr,
    NativePtr baseColorPtr, NativePtr normalPtr,
    NativePtr mrPtr, NativePtr emissivePtr)
:material(createPBRMaterial(rctxPtr, shaderPtr,
    baseColorPtr, normalPtr, mrPtr, emissivePtr))
{
    AppTrace("  PBRMaterial: Ptr: {}", material);
}
