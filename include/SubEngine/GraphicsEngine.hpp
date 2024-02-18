#pragma once

#include <set>

#include "Observer.hpp"
#include "Skin.hpp"
#include "SubEngine.hpp"

namespace Game{
    namespace SubEngine{
        class GraphicsEngine: public Interface,
            public Observer<Skin::Flyweight::ColorRect>
        {
        private:
            using ptr=std::weak_ptr<Component::Drawable>;
            struct DrawOrder {
                bool operator()(const ptr& lhs, const ptr& rhs) const;
            };

        public:
            GraphicsEngine() noexcept=default;
            virtual ~GraphicsEngine() = default;

            void update(const Time& deltaTime) noexcept override final;
        
            Type getType() const noexcept override final{
                return Type::GraphicsEngine;
            }
            void append(ptr object) noexcept;

        private:
            virtual void prepareRendering() noexcept=0;
            virtual void finalizeRendering() noexcept=0;

        private:
            // ordered by Draw Order(Draw Component's);
            std::multiset<ptr, DrawOrder> drawables;
        };
    }
}

namespace Null{
    namespace SubEngine{
        class GraphicsEngine: public Game::SubEngine::GraphicsEngine{
        private:
            void onNotify(const Skin::Flyweight::ColorRect& rect
            ) noexcept override final;

            void prepareRendering() noexcept final{}
            void finalizeRendering() noexcept final{}
        };
    }
}
