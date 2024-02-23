#pragma once

#include <memory>
#include "TinyTraits.hpp"
#include "myfwd.hpp"

namespace Engine{
    class SoundEngine: public Singleton<SoundEngine>{
        friend class ::MainEngine;

      public:
        SoundEngine() noexcept;
        virtual ~SoundEngine();

      private:
        std::unique_ptr<BindedLogger> logger;
    };
}