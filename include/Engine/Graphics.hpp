#pragma once

#include <memory>
#include <set>

#include "TinyTraits.hpp"
#include "Skin.hpp"
#include "myfwd.hpp"

namespace Engine{
    class Graphics: public Singleton<Graphics>{
        friend class Core;

        using wp=std::weak_ptr<Component::Drawable>;

      public:
        Graphics() noexcept;
        virtual ~Graphics();

        void update(const Game::Time&) noexcept;

        void append(wp) noexcept;
        // void remove(std::weak_ptr<Component::Drawable>) noexcept;

        virtual void initialize() noexcept=0;
        virtual void destroyAll() noexcept=0;
        virtual void* context() noexcept=0;

        virtual void draw(::Skin::Flyweight::ColorRect) noexcept=0;

        virtual void clearScreen() noexcept=0;
        virtual void swapBuffer() noexcept=0;

      private:
        std::unique_ptr<BindedLogger> logger;

        struct DrawOrder{
            bool operator()(const wp& lhs, const wp& rhs) const;
        };
        std::multiset<wp, DrawOrder> drawables;
    };
}
