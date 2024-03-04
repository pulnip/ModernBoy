#pragma once

#include <string>
#include "Observer.hpp"
#include "TinyTraits.hpp"
#include "myfwd.hpp"
#include "Blueprint.hpp"
#include "Engine/Logger.hpp"

namespace Engine{
    extern std::string title;
    extern Blueprint::Screen screen;

    class Core: public MakableSingleton<Core>,
        public Receiver
    {
        friend class ::MainEngine;
      public:
        Core(std::unique_ptr<Timer>, const ::Blueprint::Window&) noexcept;
        virtual ~Core()=default;

        void run() noexcept;

      private:
        void handler() noexcept override final{
            isRunning=false;
        }

      protected:
        bool isRunning=false;
        ::Blueprint::Window window={};

      private:
        ::Logger::Binded logger={"Core", "base"};
        std::unique_ptr<Timer> timer;
    };
}