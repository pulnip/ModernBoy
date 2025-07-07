#include <variant>
#include <SDL3/SDL_log.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#define IMGUI_IMPL_METAL_CPP
#include <imgui_impl_metal.h>
#include "ui/user_interface.hpp"
#include "app_state.hpp"

using namespace ModernBoy;
using namespace ModernBoy::UI;

UserInterface::UserInterface(AppState& app,
    SDL_Window* window
):app(app){
    int w, h;
    if(!SDL_GetWindowSize(window, &w, &h)){
        SDL_Log("SDL_GetWindowSize Failed: %s", SDL_GetError());
        throw 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(w, h);
    // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    // ImGui::StyleColorsLight();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForMetal(window);
    auto device = static_cast<MTL::Device*>(
        app.getDevice());
    ImGui_ImplMetal_Init(device);

    // move to somewhere
    auto id = emplace<MenuController>();

    using namespace std::string_literals;

    emplace<CheckBox>(id, "WireFrame"s, false);
    emplace<SameLine>(id);
    emplace<CheckBox>(id, "DrawNormal"s, false);
    emplace<CheckBox>(id, "usePerspective"s, false);
    emplace<Slider>(id, "Normal Scale"s, 0.0f, 1.0f, 0.5f);
    emplace<RadioButton>(id, std::vector{
        "DirLight"s, "PointLight"s, "SpotLight"s}, 0);
    emplace<Slider>(id, "Rim Power"s, 0.0f, 10.0f, 0.0f);
    emplace<Slider>(id, "Rim Strength"s, 0.0f, 1.0f, 0.0f);
}

size_t UserInterface::yield_count() const noexcept{
    return controllers.size();
}

void UserInterface::onFrameStart(){
    auto renderPassDesc = static_cast<MTL::RenderPassDescriptor*>(
        app.getRenderPassDesc()
    );
    // Start the Dear ImGui frame
    ImGui_ImplMetal_NewFrame(renderPassDesc);
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();
}
Generator<void> UserInterface::updateTask(DeltaTime dt){
    // ImGui::ShowDemoWindow(); // Show demo window! :)s
    for(auto& [id, ctrller]: controllers){
        std::visit([dt](auto& controller){
            controller.update(dt);
        }, ctrller);
        co_yield 0;
    }
    co_return;
}
void UserInterface::onFrameEnd(){
    ImGui::EndFrame();
    ImGui::Render();

    auto commandBuffer = app.getCommandBuffer();
    auto renderEncoder = app.getRenderEncoder();

    assert(commandBuffer != nullptr);
    assert(renderEncoder != nullptr);
    ImGui_ImplMetal_RenderDrawData(
        ImGui::GetDrawData(),
        static_cast<MTL::CommandBuffer*>(commandBuffer),
        static_cast<MTL::RenderCommandEncoder*>(renderEncoder)
    );
}

void UserInterface::handleEvent(Event event){
    auto it = control2Controller.find(event.id);
    if(it == control2Controller.cend())
        return;
    auto ctrllerID = it->second;
    std::visit([&](auto& controller){
        controller.OnEvent(event);
    }, controllers.at(ctrllerID));
}

uint32_t UserInterface::issueID() const noexcept{
    return app.issueID();
}
