#include "Engine/JsonHelper.hpp"
#include "Math.hpp"
#include <iostream>

using namespace My::Math;
using namespace JsonHelper;
using namespace CustomHelper;
using namespace ActorHelper;

std::optional<int> JsonHelper::getInt(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());

    if(it == object.MemberEnd() or not it->value.IsInt()){
        return std::nullopt;
    }

    return it->value.GetInt();
}

std::optional<double> JsonHelper::getDouble(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());

    if(it==object.MemberEnd() or not it->value.IsDouble()){
        return std::nullopt;
    }

    return it->value.GetDouble();
}

std::optional<JsonHelper::Array> JsonHelper::getArray(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());

    if(it==object.MemberEnd() or not it->value.IsArray()){
        return std::nullopt;
    }

    return it->value.GetArray();
}

std::optional<Game::Vector2D> CustomHelper::getVector2D(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());
    if(it==object.MemberEnd() or not it->value.IsObject()){
        return std::nullopt;
    }

    auto vector2d=it->value.GetObject();

    auto x=getDouble(vector2d, "x");
    auto y=getDouble(vector2d, "y");
    if(not x.has_value() or not y.has_value()){
        return std::nullopt;
    }

    return Game::Vector2D{x.value(), y.value()};
}

std::optional<Skin::TrueColor> CustomHelper::getColor(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());
    if(it==object.MemberEnd() or not it->value.IsObject()){
        return std::nullopt;
    }

    auto color=it->value.GetObject();

    auto red=getInt(color, "red");
    auto green=getInt(color, "green");
    auto blue=getInt(color, "blue");
    auto alpha=getInt(color, "alpha");
    if( not red.has_value() or
        not green.has_value() or
        not blue.has_value() or
        not alpha.has_value()
    ){
        return std::nullopt;
    }

    return Skin::TrueColor{
        static_cast<unsigned char>(red.value()),
        static_cast<unsigned char>(green.value()),
        static_cast<unsigned char>(blue.value()),
        static_cast<unsigned char>(alpha.value())
    };
}

std::optional<Blueprint::Actor> ActorHelper::getActor(
    const rapidjson::Value& object
) noexcept{
    auto position=getVector2D(object, "position");
    auto size=getVector2D(object, "size");
    auto color=getColor(object, "color");

    if(not position.has_value()){
        std::cout<<"not pos\n";
    }
    if(not size.has_value()){
        std::cout<<"not size\n";
    }
    if(not color.has_value()){
        std::cout<<"not color\n";
    }

    if( not position.has_value() or
        not size.has_value() or
        not color.has_value()
    ){
        return std::nullopt;
    }

    Blueprint::Actor blueprint;

    blueprint.physicsAttribute.position.linear = position.value();
    blueprint.physicsAttribute.volume.base = size.value();
    blueprint.color = color.value();

    return blueprint;
}
