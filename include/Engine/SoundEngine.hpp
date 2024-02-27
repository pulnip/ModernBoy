#pragma once

#include <memory>
#include "TinyTraits.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"

namespace Engine{
    class SoundEngine: public Singleton<SoundEngine>{
        friend class ::MainEngine;

      public:
        virtual ~SoundEngine()=default;

      private:
        ::Logger::Binded logger={"SoundEngine", "base"};
    };
}