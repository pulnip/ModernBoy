#ifndef MODERNBOY_APP_STATE_HPP
#define MODERNBOY_APP_STATE_HPP

#include "alias.hpp"
#include "../../game/src/context.hpp"
#include "event.hpp"
#include "resource_manager.hpp"
#include "fwd.hpp"
#include "asset/asset_loader.hpp"
#include "asset/scene_loader.hpp"
#include "script/type.hpp"
#include "script/invoker.hpp"
#include "input/device.hpp"
#include "render/renderer.hpp"
#include "ui/user_interface.hpp"
#include "interface/input_service.hpp"
#include "interface/view_service.hpp"
#include "interface/draw_service.hpp"
#include "interface/engine_command_bus.hpp"
#include "scheduler.hpp"
#include "event.hpp"
#if defined(USE_DIRECTX)
#include "render/dx11/mesh.hpp"
#include "render/dx11/texture.hpp"
#include "render/dx11/shader.hpp"
#elif defined(USE_METAL)
#include "render/metal/material.hpp"
#include "render/metal/submesh.hpp"
#include "render/metal/shader.hpp"
#elif defined(USE_OPENGL)
#include "render/opengl/mesh.hpp"
#endif

namespace ModernBoy
{
    class Engine{
    public:
        Engine(SDL_Window* window);
        ~Engine() = default;

        void start();
        void shutdown();

        void update();

        EntityID issueID();
        template<Game::Event event>
        void on();
        template<Event event>
        void on();

        DeltaTime getDeltaTime() const;

        // AssetLoader helper
        Script::FunctionID registerFunction(const std::string& funcName);
        // UserInterface helper
#if defined(USE_DIRECTX)
        NativePtr getDevice();
        NativePtr getContext();
#elif defined(USE_METAL)
        NativePtr getRenderPassDesc();
        NativePtr getDevice();
        NativePtr getCommandBuffer();
        NativePtr getRenderEncoder();
#endif
    private:
        void operator()(Interface::SetMouseMode);

    private:
        EntityID id_seed = 0;

        SDL_Window* window;
#if defined(USE_DIRECTX)
#elif defined(USE_METAL)
        NativePtr metalView;
#elif defined(USE_OPENGL)
#endif
        // Resource Managers
        SubmeshManager submeshManager;
        MaterialManager materialManager;

        MeshTable meshTable;
        MaterialSetTable materialSetTable;
        ShaderManager shaderManager;
        ModuleManager moduleManager;
        ObjectManager objectManager;

        Interface::InputService inputService;
        Interface::ViewService viewService;
        Interface::DrawService drawService;
        Interface::EngineCommandBus engineCommandBus;

        // App Subsystem
        Render::Renderer renderer;
        UI::UserInterface userInterface;
        Input::Device inputDevice;
        Script::Invoker scriptInvoker;

        Game::Context world;
        Asset::AssetLoader assetLoader;
        Asset::SceneLoader sceneLoader;

        Timepoint lastTick;
        DeltaTime deltaTime;
        // static constexpr auto NUM_SCHEDULING = 4;
        // std::vector<Generator<void>> generators;
        // std::vector<Index> schedule;

    public:
        friend Asset::AssetLoader;
        friend Game::Context;
        friend Script::TypeHelper;
    };

    Engine createEngine();
}

#endif // MODERNBOY_APP_STATE_HPP