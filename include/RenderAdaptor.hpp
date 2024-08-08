#pragma once

#include <memory>
#include "Camera.hpp"
#include "Info.hpp"

namespace ModernBoy{
    class RenderAdaptor{
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        struct ShaderAdaptor;
        std::unique_ptr<ShaderAdaptor> shader;

      protected:
        Camera camera;
        float xSplit=0.0f;

      public:
        RenderAdaptor(const WindowDesc& wd);
        virtual ~RenderAdaptor();

        bool run();

      private:
        virtual void updateGUI()=0;
        virtual void update(float dt)=0;
    };
}
