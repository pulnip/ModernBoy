#pragma once

#include <memory>
#include <optional>
#include <string>
#include <rapidjson/document.h>
#include "Skin.hpp"
#include "Actor/Vanilla.hpp"

namespace JsonHelper{
    std::optional<int> getInt(
        const rapidjson::Value& object, const std::string& property
    ) noexcept;
    std::optional<double> getDouble(
        const rapidjson::Value& object, const std::string& property
    ) noexcept;
    std::optional<std::string> getString(
        const rapidjson::Value& object, const std::string& property
    ) noexcept;
    using Array=rapidjson::GenericArray<true, rapidjson::Value>;
    std::optional<Array> getArray(
        const rapidjson::Value& object, const std::string& property
    ) noexcept;
    std::optional<rapidjson::GenericObject<true, rapidjson::Value>> getObject(
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
    std::optional<std::shared_ptr<Actor::Vanilla>> getActor(
        const rapidjson::Value& object
    ) noexcept;
}