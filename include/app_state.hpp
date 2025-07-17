#ifndef MODERNBOY_APP_STATE_HPP
#define MODERNBOY_APP_STATE_HPP

#include <SDL3/SDL_video.h>
#include "common/alias.hpp"
#include "asset_loader.hpp"
#include "game/context.hpp"
#include "game/event.hpp"
#include "resource_manager.hpp"
#include "script/type.hpp"
#include "script/invoker.hpp"
#include "input/chord.hpp"
#include "render/renderer.hpp"
#include "ui/user_interface.hpp"
#include "scheduler.hpp"
#include "event.hpp"
#if defined(USE_DIRECTX)
#include "backends/dx11/mesh.hpp"
#elif defined(USE_METAL)
#include "SDL3/SDL_metal.h"
#include "backends/metal/mesh.hpp"
#include "backends/metal/texture.hpp"
#include "backends/metal/shader.hpp"
#elif defined(USE_OPENGL)
#include "backends/opengl/mesh.hpp"
#endif

namespace ModernBoy
{
    using MeshManager = ResourceManager<Mesh>;
    using TextureManager = ResourceManager<Texture>;
    using ShaderManager = ResourceManager<Shader>;
    using ModuleManager = ResourceManager<Script::Module>;

    class AppState{
    public:
        AppState(SDL_Window* window);
        ~AppState() = default;
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
        // append by resource filename
        template<typename Resrc>
        uint32_t append(const std::string&);

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

        // get task of Subsystem
        template<typename Task>
        const std::vector<Task>& getBuffer() const{
            return world.getBuffer<Task>();
        }

        DeltaTime getDeltaTime() const;

        // AssetLoader helper
        FunctionID registerFunction(const std::string& funcName);
        // UserInterface helper
        NativePtr getRenderPassDesc();
        NativePtr getDevice();
        NativePtr getCommandBuffer();
        NativePtr getRenderEncoder();

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

        // App Subsystem
        Render::Renderer renderer;
        UI::UserInterface userInterface;
        Input::Chord inputChord;
        Script::Invoker scriptInvoker;

        Game::Context world;
        AssetLoader assetLoader;

        Timepoint lastTick;
        DeltaTime deltaTime;
        static constexpr auto NUM_SCHEDULING = 4;
        std::vector<Generator<void>> generators;
        std::vector<Index> schedule;

    public:
        friend AssetLoader;
        friend Game::Context;
    };

    AppState createAppState();
}

#endif // MODERNBOY_APP_STATE_HPP