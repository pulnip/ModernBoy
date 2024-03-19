#pragma once

#include <memory>
#include <set>
#include <string_view>
#include "Blueprint.hpp"
#include "TinyTraits.hpp"
#include "Skin.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"

namespace Engine{
    class Graphics: public MakableSingleton<Graphics, false, false>{
        friend class Core;

        using wp=std::weak_ptr<Component::Drawable>;

      public:
        virtual ~Graphics()=default;

        void update(const Game::Time&) noexcept;

        void append(wp) noexcept;
        // void remove(std::weak_ptr<Component::Drawable>) noexcept;

        virtual void initialize(const ::Blueprint::Window&) noexcept;
        virtual void destroyAll() noexcept=0;
        virtual void* context() noexcept=0;

        virtual void draw(::Skin::Flyweight::ColorRect) noexcept=0;

        virtual void clearScreen() noexcept=0;
        virtual void swapBuffer() noexcept=0;

        std::string_view getTitle() noexcept{ return windowSetting.title; }
        auto getWindowPosition() noexcept{ return windowSetting.screen.position; }
        auto getWindowSize() noexcept{ return windowSetting.screen.size; }

      private:
        ::Logger::Binded logger={"Graphics", "base"};
        ::Blueprint::Window windowSetting={};

        struct DrawOrder{
            bool operator()(const wp& lhs, const wp& rhs) const;
        };
        std::multiset<wp, DrawOrder> drawables;
    };
}
