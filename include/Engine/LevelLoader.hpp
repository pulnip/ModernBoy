#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <rapidjson/document.h>
#include "Blueprint.hpp"
#include "TinyTraits.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"

namespace Engine{
    class LevelLoader: public Singleton<LevelLoader>{
        friend class ::MainEngine;
      public:
        virtual ~LevelLoader()=default;

        virtual Blueprint::Setting loadLevel(
            const std::string& fileName
        ) noexcept=0;

      private:
        Logger::Binded logger={"LevelLoader", "base"};
    };
}

namespace WithRapidjson{
    class LevelLoader: public ::Engine::LevelLoader{
      public:
        Blueprint::Setting loadLevel(
            const std::string& fileName
        ) noexcept override final;

      private:
        public:
        // parse json file to rapidjson::Document
        std::optional<rapidjson::Document> loadJson(
            const std::string& fileName
        ) noexcept;

      private:
        Logger::Binded logger={"LevelLoader", "Rapidjson"};
    };
}
