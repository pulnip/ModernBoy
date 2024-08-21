#pragma once

#include <memory>
#include <vector>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include "fwd.hpp"

namespace ModernBoy{
    class Window{
        SDL_Window* window=nullptr;

        std::unique_ptr<ShaderAdaptor> shader;
        std::unique_ptr<TextureAdaptor> texturer;
        std::unique_ptr<GUIAdaptor> gui;

        std::vector<std::shared_ptr<MeshComponent>> meshes;
        std::vector<std::shared_ptr<LightComponent>> lights;

      public:
        std::unique_ptr<RenderAdaptor> renderer;

        float deltaTime=0.0f;

      public:
        Window(const Core& core, const WindowDesc& wd);
        virtual ~Window();

        void process(const SDL_WindowEvent& event);
        void update();

        bool isMinimized();
        void setMainCamera(std::shared_ptr<CameraComponent> camera);
        void addLight(std::shared_ptr<LightComponent> light);
        void addMesh(std::shared_ptr<MeshComponent> mesh);
    };
}
