#include <print>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_dx11.h>
#include "GUIAdaptor.hpp"
#include "helper.hpp"

using namespace std;
using namespace ModernBoy;

GUIAdaptor::GUIAdaptor(SDL_Window* window,
    const ComPtr<ID3D11Device>& device,
    const ComPtr<ID3D11DeviceContext>& context,
    const ipoint2& screenSize
){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io=ImGui::GetIO(); (void)io;
    io.DisplaySize=ImVec2(screenSize.x, screenSize.y);
    // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    throwIfTrue(!ImGui_ImplSDL2_InitForD3D(window),
        "ImGui_SDL2 Init Failed."
    );
    throwIfTrue(!ImGui_ImplDX11_Init(device.Get(), context.Get()),
        "ImGui_DX11 Init Failed."
    );
}

GUIAdaptor::~GUIAdaptor(){
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void GUIAdaptor::update(){
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

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

    deltaTime=ImGui::GetIO().DeltaTime;
}

void GUIAdaptor::render(){
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
