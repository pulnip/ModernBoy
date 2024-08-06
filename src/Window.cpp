#include <imgui.h>
#include "Window.hpp"

using namespace DirectX;
using namespace ModernBoy;

void Window::updateGUI(){
    // ImGui::Checkbox("usePerspective", &shader->usePerspective);
    static float fov=70.0f;
    ImGui::SliderFloat("Field of View", &fov, 0.0f, 120.0f);

    camera.setPerspective(XMConvertToRadians(fov));
}

void Window::update([[maybe_unused]] float dt){
    camera.setEyePos({0.0f, 0.0f, -1.0f});
    camera.setEyeDir(Vector3::UnitZ);
    camera.setUpDir(Vector3::UnitY);
}
