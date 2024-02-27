#pragma once

#include <memory>
#include <optional>
#include <string>

#include <rapidjson/document.h>
#include "myfwd.hpp"
#include "TinyTraits.hpp"
#include "Engine/Logger.hpp"

namespace Engine{
    class LevelLoader:
        public Singleton<LevelLoader>,
        public Logger::Binded
    {
      public:
        LevelLoader() noexcept;
        virtual ~LevelLoader();

        bool loadLevel(
            const std::string& fileName
        ) noexcept;

      private:
    public:
        // parse json file to rapidjson::Document
        std::optional<rapidjson::Document> loadJson(
            const std::string& fileName
        ) noexcept;
    };
}
