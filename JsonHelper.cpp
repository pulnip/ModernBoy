#include "JsonHelper.hpp"

std::pair<bool, std::optional<int>> JsonHelper::getInt(
    const rapidjson::Value& object, const std::string& property
) noexcept{
    auto it=object.FindMember(property.c_str());

    if(it == object.MemberEnd()){
        return {false, std::nullopt};
    }
    if(not it->value.IsInt()){
        return {true, std::nullopt};
    }

    return {true, it->value.GetInt()};
}