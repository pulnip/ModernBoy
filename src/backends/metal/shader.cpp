#include <utility>
#include "render/gui.hpp"
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
    NativePtr layerPtr, UI* gui)
:shaderPtr(createShader(fileName.c_str(), layerPtr)), gui(gui){
    if(gui != nullptr){
        rp_id = gui->subscriberimPower([this](float rp){
            Shader_setRimPower(shaderPtr, rp);
        });
        rs_id = gui->subscriberimStrength([this](float rs){
            Shader_setRimStrength(shaderPtr, rs);
        });
    }
}
Shader::~Shader(){
    if(gui != nullptr){
        gui->unsubscriberimPower(rp_id);
        gui->unsubscriberimStrength(rs_id);
    }

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
    gui = other.gui;
    rp_id = other.rp_id;
    rs_id = other.rs_id;

    other.shaderPtr = nullptr;
    other.gui = nullptr;
    other.rp_id = -1;
    other.rs_id = -1;
}
