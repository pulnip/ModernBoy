#pragma once

#include "Observable.hpp"
#include "Drawable.hpp"

namespace Game{
    namespace Component{
        // Color Box 텍스처
        class ColoredBody final: public Drawable{
            using pColor=std::shared_ptr<Skin::TrueColor>;
        public:
            ColoredBody() noexcept: Drawable(200){}
            ~ColoredBody()=default;

            void setTexture(pColor color) noexcept;

        private:
            void postConstruct() noexcept override final;
            void tryDraw() noexcept override final;

        private:
            pColor color;
        };
    }
}
