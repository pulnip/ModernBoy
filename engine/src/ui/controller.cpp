#include "imgui.h"
#include "ui/controller.hpp"
#include "ui/user_interface.hpp"

using namespace ModernBoy;
using namespace ModernBoy::UI;

MenuController::MenuController(){
    
}

void MenuController::update(DeltaTime){
    // ImGui::ShowDemoWindow(); // Show demo window! :)

    ImGui::Begin("Scene Control");
    ImGui::Text("%.2f FPS", ImGui::GetIO().Framerate);

    for(auto& control: controls){
        std::visit([](auto& control){
            control.draw();
        }, control);
    }

    ImGui::End();
}

void MenuController::OnEvent(Event event){
    auto it = id2index.find(event.id);
    if(it == id2index.end())
        return;
    
    std::visit([&](auto& control){
        control.OnEvent(event);
    }, controls[it->second]);
}
