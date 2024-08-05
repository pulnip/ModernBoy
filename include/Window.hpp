#pragma once

#include <memory>
#include "Info.hpp"

namespace ModernBoy{
    class RenderAdaptor{
        struct Impl;
        std::unique_ptr<Impl> pImpl;
      protected:
        struct ShaderAdaptor;
        std::unique_ptr<ShaderAdaptor> shader;

      public:
        RenderAdaptor(const WindowDesc& wd);
        virtual ~RenderAdaptor();

        bool run();

      private:
        virtual void updateGUI()=0;
        virtual void update(float dt)=0;
    };

    class Window: public RenderAdaptor{        
      public:
        Window(const WindowDesc& wd)
        : RenderAdaptor(wd){}

      private:
        void updateGUI() override;
        void update(float dt) override;
    };
}
