#pragma once

#include <memory>
#include <optional>
#include <string>

#include <rapidjson/document.h>

class LevelLoader{
  public:
    LevelLoader(class MainEngine& me): mainEngine(me){}

    bool loadLevel(
        class GameEngine* gameEngine,
        const std::string& fileName
    ) noexcept;

  private:
    MainEngine& mainEngine;

    // parse json file to rapidjson::Document
    std::optional<rapidjson::Document> loadJson(
        const std::string& fileName
    ) noexcept;
};
