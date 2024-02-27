#pragma once

#include <memory>
#include "TinyTraits.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"

namespace Engine{
    class ResourceManager: public Singleton<ResourceManager>{
        friend class ::MainEngine;

      public:
        virtual ~ResourceManager()=default;

      private:
        ::Logger::Binded logger={"ResourceManager", "base"};
    };
}