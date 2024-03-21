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
    class LevelLoader: public MakableSingleton<LevelLoader, false, false>{
        friend class ::MainEngine;
      public:
        virtual ~LevelLoader()=default;

        virtual std::optional<Blueprint::Setting> loadLevel(
            const std::string& fileName
        ) noexcept=0;

      private:
        Logging::Bind logger={"LevelLoader", "base"};
    };
}

namespace WithRapidjson{
    class LevelLoader: public ::Engine::LevelLoader{
      public:
        std::optional<Blueprint::Setting> loadLevel(
            const std::string& fileName
        ) noexcept override final;

        static void make() noexcept{
            ::Engine::LevelLoader::make<LevelLoader>();
        }


      private:
        // parse json file to rapidjson::Document
        std::optional<rapidjson::Document> loadJson(
            const std::string& fileName
        ) noexcept;
        std::optional<Blueprint::Window> loadWindow(
            const rapidjson::Value& doc
        ) noexcept;

      private:
        Logging::Bind logger={"LevelLoader", "Rapidjson"};
    };
}
