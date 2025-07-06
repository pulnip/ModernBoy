#include "imgui.h"
#include "ui/controller.hpp"
#include "ui/user_interface.hpp"

using namespace ModernBoy;
using namespace ModernBoy::UI;

MenuController::MenuController()=default;

void MenuController::update(DeltaTime){
    // ImGui::ShowDemoWindow(); // Show demo window! :)

    ImGui::Begin("Scene Control");
    ImGui::Text("%.2f FPS", ImGui::GetIO().Framerate);

    for(auto& [id, ctrl]: controls){
        std::visit([](auto& control){
            control.draw();
        }, ctrl);
    }

    ImGui::End();
}

void MenuController::OnEvent(Event event){
    auto it = controls.find(event.id);
    if(it == controls.end())
        return;
    std::visit([&](auto& control){
        control.OnEvent(event);
    }, it->second);
}
