#pragma once

#include "Drawable.hpp"

namespace Game{
    namespace Component{
        // Color Box 텍스처
        class ColoredBody final: public Drawable{
        public:
            ColoredBody() noexcept: Drawable(200){}
            ~ColoredBody()=default;

            void setTexture(const Skin::TrueColor& color) noexcept;

        private:
            void tryDraw() noexcept override final;

        private:
            std::shared_ptr<Skin::TrueColor> color;
        };
    }
}
