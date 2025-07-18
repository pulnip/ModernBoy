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

        RenderContext(SDL_Window*);
        ~RenderContext();

        void onFrameStart(Vec4 clearColor);
        void setView(float fov,
            Vec3 viewPos, Vec4 viewQuat);
        void setShader(NativePtr shader);
        void setTexture(NativePtr texture);
        void drawMesh(Vec3 position, Vec4 rotation,
            Vec3 scale, NativePtr mesh);
        void onFrameEnd();

        NativePtr getRenderPassDesc();
        NativePtr getDevice();
        NativePtr getCommandBuffer();
        NativePtr getRenderEncoder();

    private:
        NativePtr _renderContext;
    };
}

#endif // MODERNBOY_METAL_CONTEXT_HPP