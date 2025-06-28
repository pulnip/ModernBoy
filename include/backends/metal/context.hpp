#ifndef MODERNBOY_METAL_CONTEXT_HPP
#define MODERNBOY_METAL_CONTEXT_HPP

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_metal.h>
#include "fwd.hpp"

namespace ModernBoy::Metal
{
    struct RenderContext{
        SDL_MetalView view;
        NativePtr metalLayer;
        NativePtr _renderContext;

        RenderContext(SDL_Window* in_window, AppState& app);
        ~RenderContext();

        void operator()(const Render::FrameStartCommand&);
        void operator()(const Render::SetViewCommand&);
        void operator()(const Render::SetShaderCommand&);
        void operator()(const Render::DrawMeshCommand&);
        void operator()(const Render::FrameEndCommand&);

    private:
        AppState& app;
    };
}

#endif // MODERNBOY_METAL_CONTEXT_HPP