#include <variant>
#include <SDL3/SDL_log.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#if defined(USE_DIRECTX)
#include <imgui_impl_dx11.h>
#elif defined (USE_METAL)
#define IMGUI_IMPL_METAL_CPP
#include <imgui_impl_metal.h>
#endif
#include "engine/render/renderer.hpp"
#include "ui/user_interface.hpp"
#include "app_state.hpp"

using namespace std::chrono_literals;
using namespace ModernBoy;
using namespace ModernBoy::UI;

UserInterface::UserInterface(SDL_Window* window,
    Render::Renderer& renderer,
    AppState& app
):renderer(renderer), app(app), ema(0ms){
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

#if defined(USE_DIRECTX)
    ImGui_ImplSDL3_InitForD3D(window);
    auto device = static_cast<ID3D11Device*>(
        renderer.getDevice());
    auto context = static_cast<ID3D11DeviceContext*>(
        renderer.getContext());
    ImGui_ImplDX11_Init(device, context);
#elif defined(USE_METAL)
    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForMetal(window);
    auto device = static_cast<MTL::Device*>(
        renderer.getDevice());
    ImGui_ImplMetal_Init(device);
#endif

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

void UserInterface::update(DeltaTime dt){
    auto started = std::chrono::steady_clock::now();

    // ImGui::ShowDemoWindow(); // Show demo window! :)s
    for(auto& [id, ctrller]: controllers){
        std::visit([dt](auto& controller){
            controller.update(dt);
        }, ctrller);
    }

    auto elapsed = std::chrono::steady_clock::now() - started;
    updateEMA(std::chrono::duration_cast<TaskTime>(elapsed));
}


// Generator<void> UserInterface::update(DeltaTime dt){
//     auto started = std::chrono::steady_clock::now();

//     // ImGui::ShowDemoWindow(); // Show demo window! :)s
//     for(auto& [id, ctrller]: controllers){
//         std::visit([dt](auto& controller){
//             controller.update(dt);
//         }, ctrller);
//         co_yield 0;
//     }

//     auto elapsed = std::chrono::steady_clock::now() - started;
//     updateEMA(std::chrono::duration_cast<TaskTime>(elapsed));
//     co_return;
// }

void UserInterface::onFrameStart(){
#if defined(USE_DIRECTX)
    ImGui_ImplDX11_NewFrame();
#elif defined(USE_METAL)
    auto renderPassDesc = static_cast<MTL::RenderPassDescriptor*>(
        renderer.getRenderPassDesc()
    );
    // Start the Dear ImGui frame
    ImGui_ImplMetal_NewFrame(renderPassDesc);
#endif
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();
}
void UserInterface::onFrameEnd(){
    ImGui::EndFrame();
    ImGui::Render();

#if defined(USE_DIRECTX)
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#elif defined(USE_METAL)
    auto commandBuffer = renderer.getCommandBuffer();
    auto renderEncoder = renderer.getRenderEncoder();

    assert(commandBuffer != nullptr);
    assert(renderEncoder != nullptr);

    ImGui_ImplMetal_RenderDrawData(
        ImGui::GetDrawData(),
        static_cast<MTL::CommandBuffer*>(commandBuffer),
        static_cast<MTL::RenderCommandEncoder*>(renderEncoder)
    );
#endif
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

void UserInterface::updateEMA(TaskTime elapsed){
    auto alpha = 1.0 / policy.effective_window_size;
    long long blended = (1-alpha)*ema.count() + alpha*elapsed.count();
    ema = TaskTime(blended);
}

uint32_t UserInterface::issueID() const noexcept{
    return app.issueID();
}
