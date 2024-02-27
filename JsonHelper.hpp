#pragma once

#include <memory>
#include <optional>
#include <string>
#include <rapidjson/document.h>
#include "Blueprint.hpp"
#include "Skin.hpp"

namespace JsonHelper{
    std::optional<int> getInt(
        const rapidjson::Value& object, const std::string& property
    ) noexcept;
    std::optional<double> getDouble(
        const rapidjson::Value& object, const std::string& property
    ) noexcept;
    using Array=rapidjson::GenericArray<true, rapidjson::Value>;
    std::optional<Array> getArray(
        const rapidjson::Value& object, const std::string& property
    ) noexcept;
}

namespace CustomHelper{
    std::optional<Game::Vector2D> getVector2D(
        const rapidjson::Value& object, const std::string& property
    ) noexcept;
    std::optional<Skin::TrueColor> getColor(
        const rapidjson::Value& object, const std::string& property
    ) noexcept;
}

namespace ActorHelper{
    std::optional<Blueprint::Actor> getActor(
        const rapidjson::Value& object
    ) noexcept;
}