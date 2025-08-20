#ifndef MODERNBOY_APP_STATE_HPP
#define MODERNBOY_APP_STATE_HPP

#include "core/alias.hpp"
#include "../../src/game/context.hpp"
#include "event.hpp"
#include "resource_manager.hpp"
#include "engine/fwd.hpp"
#include "engine/asset/asset_loader.hpp"
#include "engine/asset/scene_loader.hpp"
#include "engine/asset_loader.hpp"
#include "engine/script/type.hpp"
#include "engine/script/invoker.hpp"
#include "engine/input/device.hpp"
#include "engine/render/renderer.hpp"
#include "engine/ui/user_interface.hpp"
#include "engine/interface/input_service.hpp"
#include "engine/interface/view_service.hpp"
#include "engine/interface/draw_service.hpp"
#include "engine/interface/engine_command_bus.hpp"
#include "scheduler.hpp"
#include "event.hpp"
#if defined(USE_DIRECTX)
#include "engine/render/dx11/mesh.hpp"
#include "engine/render/dx11/texture.hpp"
#include "engine/render/dx11/shader.hpp"
#elif defined(USE_METAL)
#include "engine/render/metal/mesh.hpp"
#include "engine/render/metal/texture.hpp"
#include "engine/render/metal/shader.hpp"
#elif defined(USE_OPENGL)
#include "engine/render/opengl/mesh.hpp"
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

        // append by resource construction argument
        template<typename Resrc, typename... Args>
        Handle append(Args&&...);

        // get resource from handle
        template<typename Resrc>
        Resrc& query(Handle);
        template<typename Resrc>
        const Resrc& query(Handle) const;

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
        MeshManager meshManager;
        TextureManager textureManager;
        MaterialTable materialTable;
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
        AssetLoader assetLoader;
        Asset::AssetLoader assetLoader2;
        Asset::SceneLoader sceneLoader;

        Timepoint lastTick;
        DeltaTime deltaTime;
        // static constexpr auto NUM_SCHEDULING = 4;
        // std::vector<Generator<void>> generators;
        // std::vector<Index> schedule;

    public:
        friend Asset::AssetLoader;
        friend AssetLoader;
        friend Game::Context;
        friend Script::TypeHelper;
    };

    Engine createEngine();
}

#endif // MODERNBOY_APP_STATE_HPP