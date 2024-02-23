#pragma once

#include <memory>
#include "TinyTraits.hpp"
#include "myfwd.hpp"

namespace Engine{
    class ResourceManager: public Singleton<ResourceManager>{
        friend class ::MainEngine;

      public:
        ResourceManager() noexcept;
        virtual ~ResourceManager();

      private:
        std::unique_ptr<BindedLogger> logger;
    };
}