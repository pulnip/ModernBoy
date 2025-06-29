#ifndef MODERNBOY_METAL_CONTEXT_HPP
#define MODERNBOY_METAL_CONTEXT_HPP

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_metal.h>
#include "fwd.hpp"

struct ImDrawData;

namespace ModernBoy::Metal
{
    struct RenderContext{
        SDL_MetalView view;
        NativePtr metalLayer;

        RenderContext(SDL_Window* window);
        ~RenderContext();

        void initialize(SDL_Window* window);
        void onFrameStart(Vec4 clearColor);
        void setView(float fov,
            Vec3 viewPos, Vec4 viewQuat);
        void setShader(NativePtr shader);
        void setTexture(NativePtr texture);
        void drawMesh(Transform transform, NativePtr mesh);
        void onFrameEnd(ImDrawData* uiDrawData);

    private:
        NativePtr _renderContext;
    };
}

#endif // MODERNBOY_METAL_CONTEXT_HPP