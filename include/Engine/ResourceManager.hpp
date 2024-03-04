#pragma once

#include <memory>
#include "TinyTraits.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"

namespace Engine{
    class ResourceManager: public MakableSingleton<ResourceManager>{
        friend class ::MainEngine;

      public:
        static void preConstruct() noexcept{}
        void postConstruct() noexcept override final{}
        virtual ~ResourceManager()=default;

      private:
        ::Logger::Binded logger={"ResourceManager", "base"};
    };
}