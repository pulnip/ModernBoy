#include "JsonHelper.hpp"
#include <format>
#include <fstream>
#include <rapidjson/error/en.h>
#include <rapidjson/error/error.h>

using namespace std;
using namespace rapidjson;
using namespace ModernBoy;

Json::Json(const string& fileName){
    ifstream file(fileName,
        ios::in | ios::binary | ios::ate
    );

    if(!file.is_open()){    
        throw ifstream::failure(
            format("{} not found.", fileName)
        );
    }

    size_t fileSize=file.tellg();
    file.seekg(0, ios::beg);

    char json[fileSize+1];
    file.read(json, fileSize);

    ParseResult ok=doc.Parse<kParseStopWhenDoneFlag>(json);
    if(!ok){
        throw std::format("JSON parse error: {} ({})",
            GetParseError_En(ok.Code()),
            ok.Offset()
        );
    }
}

Json::operator JsonHelper(){
    return {doc};
}

JsonHelper::JsonHelper(const Value& object)
:object(object){}

optional<int> JsonHelper::getInt(const string& propertyName){
    const auto it=object.FindMember(propertyName.c_str());
    if(it==object.MemberEnd()){
        return nullopt;
    }

    const auto& property=it->value;
    if(!property.IsInt()){
        throw std::runtime_error(
            std::format("{} NOT int", propertyName)
        );
    }

    return property.GetInt();
}

optional<double> JsonHelper::getDouble(const string& propertyName){
    const auto it=object.FindMember(propertyName.c_str());
    if(it==object.MemberEnd()){
        return nullopt;
    }

    const auto& property=it->value;
    if(!property.IsDouble()){
        throw std::runtime_error(
            std::format("{} NOT double", propertyName)
        );
    }

    return property.GetDouble();
}

optional<string> JsonHelper::getString(const string& propertyName){
    const auto it=object.FindMember(propertyName.c_str());
    if(it==object.MemberEnd()){
        return nullopt;
    }

    const auto& property=it->value;
    if(!property.IsString()){
        throw std::runtime_error(
            std::format("{} NOT string", propertyName)
        );
    }

    return property.GetString();
}

optional<JsonHelper> JsonHelper::subset(const string& propertyName){
    const auto it=object.FindMember(propertyName.c_str());
    if(it==object.MemberEnd()){
        return nullopt;
    }

    const auto& property=it->value;
    if(!property.IsObject()){
        throw std::runtime_error(
            std::format("{} NOT object", propertyName)
        );
    }

    return JsonHelper(property.GetObject());
}

optional<Vector2> JsonHelper::getVector2(const string& propertyName){
    const auto it=object.FindMember(propertyName.c_str());
    if(it==object.MemberEnd()){
        return nullopt;
    }

    const auto& property=it->value;
    if(property.Size()!=2){
        throw length_error(
            format("{} length NOT 2", propertyName)
        );
    }

    const auto& x=property[0];
    const auto& y=property[1];

    if(!(x.IsFloat() && y.IsFloat())){
        throw bad_cast();
    }

    return Vector2{
        property[0].GetFloat(),
        property[1].GetFloat()
    };
}
