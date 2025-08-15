#ifndef MODERNBOY_APP_STATE_HPP
#define MODERNBOY_APP_STATE_HPP

#include <SDL3/SDL_video.h>
#include "core/alias.hpp"
#include "asset_loader.hpp"
#include "../../src/game/context.hpp"
#include "event.hpp"
#include "resource_manager.hpp"
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
#include "SDL3/SDL_metal.h"
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
        void shutdown();

        void update();

        EntityID issueID();
        template<Game::Event event>
        void on();
        template<Event event>
        void on();

        // append by resource construction argument
        template<typename Resrc, typename... Args>
        ResourceHandle append(Args&&...);
        template<typename Resrc, typename... Args>
        HandleV2 appendV2(Args&&...);
        // append by resource filename
        // template<typename Resrc>
        // uint32_t append(const std::string&);

        // get resource from handle
        template<typename Resrc>
        Resrc& query(ResourceHandle);
        template<typename Resrc>
        const Resrc& query(ResourceHandle) const;
        // get handle from resource name
        template<typename Resrc>
        ResourceHandle query(const std::string&);

        // unlink handle from resource
        template<typename Resrc>
        ABNORMAL_FLAG remove(ResourceHandle);

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
        MeshManager meshManager;
        TextureManager textureManager;
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

        Timepoint lastTick;
        DeltaTime deltaTime;
        // static constexpr auto NUM_SCHEDULING = 4;
        // std::vector<Generator<void>> generators;
        // std::vector<Index> schedule;

    public:
        friend AssetLoader;
        friend Game::Context;
        friend Script::TypeHelper;
    };

    Engine createEngine();
}

#endif // MODERNBOY_APP_STATE_HPP