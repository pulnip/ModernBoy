#include <imgui.h>
#include "render/gui.hpp"

using namespace ModernBoy;

void UI::update(){
    ImGui::Begin("Scene Control");

    ImGui::Text("%.2f FPS", ImGui::GetIO().Framerate);

    ImGui::Checkbox("WireFrame", &drawAsWire);
    // ImGui::SameLine();
    // ImGui::Checkbox("DrawNormal", &drawNormal);

    // ImGui::Checkbox("usePerspective", &shader->usePerspective);
    ImGui::SliderFloat("Field of View", &fov, 0.0f, 120.0f);
    ImGui::SliderFloat("Normal Scale", &normalScale, 0.0f, 1.0f);

    if(ImGui::RadioButton("DirLight", lightType==0))
        lightType=0;
    ImGui::SameLine();
    if(ImGui::RadioButton("PointLight", lightType==1))
        lightType=1;
    ImGui::SameLine();
    if(ImGui::RadioButton("SpotLight", lightType==2))
        lightType=2;

    ImGui::SliderFloat("Rim Strength", &rimStrength, 0.0f, 5.0f);
    ImGui::SliderFloat("Rim Power", &rimPower, 0.0f, 10.0f);

    ImGui::End();
}