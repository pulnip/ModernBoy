#pragma once

#include <memory>
#include "Attributes.hpp"
#include "Camera.hpp"
#include "Info.hpp"

namespace ModernBoy{
    class RenderAdaptor{
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        struct ShaderAdaptor;
        std::unique_ptr<ShaderAdaptor> shader;
        struct TextureAdaptor;
        std::unique_ptr<TextureAdaptor> texturer;

      protected:
        Camera camera;
        [[maybe_unused]] float xSplit=0.0f;
        Light light{};
        int lightType=0;

      public:
        RenderAdaptor(const WindowDesc& wd);
        virtual ~RenderAdaptor();

        bool run();

      private:
        virtual void updateGUI()=0;
        virtual void update(float dt)=0;
    };
}
