#pragma once

#include "RenderAdaptor.hpp"

namespace ModernBoy{
    class Window: public RenderAdaptor{        
      public:
        Window(const WindowDesc& wd)
        : RenderAdaptor(wd){}

      private:
        void updateGUI() override;
        void update(float dt) override;
    };
}
