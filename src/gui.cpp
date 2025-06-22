#include <utility>
#include <imgui.h>
#include "render/gui.hpp"

using namespace ModernBoy;

void UI::update(){
    ImGui::Begin("Scene Control");

    ImGui::Text("%.2f FPS", ImGui::GetIO().Framerate);

    if(ImGui::Checkbox("WireFrame", &drawAsWire))
        notifydrawAsWire();
    // ImGui::SameLine();
    // ImGui::Checkbox("DrawNormal", &drawNormal);

    // ImGui::Checkbox("usePerspective", &shader->usePerspective);
    if(ImGui::SliderFloat("Field of View", &fieldOfView, 0.0f, 120.0f))
        notifyfieldOfView();
    if(ImGui::SliderFloat("Normal Scale", &normalScale, 0.0f, 1.0f))
        notifynormalScale();

    if(ImGui::RadioButton("DirLight", lightType==0)){
        lightType=0;
        notifylightType();
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("PointLight", lightType==1)){
        lightType=1;
        notifylightType();
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("SpotLight", lightType==2)){
        lightType=2;
        notifylightType();
    }

    if(ImGui::SliderFloat("Rim Power", &rimPower, 0.0f, 10.0f)){
        notifyrimPower();
    }
    if(ImGui::SliderFloat("Rim Strength", &rimStrength, 0.0f, 5.0f))
        notifyrimStrength();

    ImGui::End();
}

#define UI_ELM_FUNC_DEFINE(e) \
ObserverID UI::subscribe##e(const Observer<decltype(e)>& o) noexcept{ \
    std::lock_guard<std::mutex> lock(e##Mutex); \
    e##Observers.emplace(std::make_pair(id_seed, o)); \
    return id_seed++; \
} \
void UI::notify##e() noexcept{ \
    std::lock_guard<std::mutex> lock(e##Mutex); \
    for(auto& o: e##Observers) o.second(e); \
} \
void UI::unsubscribe##e(ObserverID id){ \
    std::lock_guard<std::mutex> lock(e##Mutex); \
    if(e##Observers.erase(id) == 0) \
        throw std::runtime_error("Invalid Observer ID!"); \
}

UI_ELM_FUNC_DEFINE(drawAsWire)
UI_ELM_FUNC_DEFINE(fieldOfView)
UI_ELM_FUNC_DEFINE(normalScale)
UI_ELM_FUNC_DEFINE(lightType)
UI_ELM_FUNC_DEFINE(rimPower)
UI_ELM_FUNC_DEFINE(rimStrength)

#undef UI_ELM_FUNC_DEFINE