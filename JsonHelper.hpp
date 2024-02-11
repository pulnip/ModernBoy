#pragma once

#include <memory>
#include <optional>
#include <string>

#include <rapidjson/document.h>

class JsonHelper{
  public:
    // return value
    // - first: true if property exist.
    // - second: valid if type correct
    std::pair<bool, std::optional<int>> getInt(
        const rapidjson::Value& object, const std::string& property
    ) noexcept;
};

class ActorHelper: public JsonHelper{

  public:
    std::optional<class ActorBlueprint> get(
        const rapidjson::Value& object
    ) noexcept;
};