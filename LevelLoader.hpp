#pragma once

#include <memory>
#include <optional>
#include <string>

#include <rapidjson/document.h>
#include "myfwd.hpp"
#include "TinyTraits.hpp"

namespace Engine{
    class LevelLoader: public Singleton<LevelLoader>{
      public:
        LevelLoader() noexcept;
        virtual ~LevelLoader();

        bool loadLevel(
            const std::string& fileName
        ) noexcept;

      private:
        // parse json file to rapidjson::Document
        std::optional<rapidjson::Document> loadJson(
            const std::string& fileName
        ) noexcept;

      private:
        std::unique_ptr<BindedLogger> logger;
    };
}
