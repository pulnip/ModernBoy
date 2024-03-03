#include <fstream>
#include <vector>
#include "Engine/LevelLoader.hpp"
#include "Engine/JsonHelper.hpp"

using namespace WithRapidjson;

std::optional<Blueprint::Setting> LevelLoader::loadLevel(
    const std::string& fileName
) noexcept{
    auto doc=loadJson(fileName);
    if(not doc.has_value()){
        logger.info("load failed");
        return std::nullopt;
    }

    auto document=std::move(doc.value());
    Blueprint::Setting result={};

    auto window=loadWindow(document);
    if(window){
        result.window=window.value();
    }

    return result;
}

std::optional<rapidjson::Document> LevelLoader::loadJson(
    const std::string& fileName
) noexcept{
    std::ifstream file(
        "C:/Users/choiw/Documents/GameEngineDevelopment/"+fileName,
        std::ios::in | std::ios::binary | std::ios::ate
    );
    if(not file.is_open()){
        logger.info(("file "+fileName+" Not Found").c_str());
        return std::nullopt;
    }

    // 파일의 크기
    size_t fileSize=file.tellg();
    // 스트림 버퍼가 파일 처음 부분을 가리키도록
    file.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize + 1);
    file.read(bytes.data(), fileSize);

    rapidjson::Document doc;
    doc.Parse(bytes.data());
    if(not doc.IsObject()){
        logger.info(("file"+fileName+"not valid json").c_str());
        return std::nullopt;
    }

    return doc;
}

std::optional<Blueprint::Window> LevelLoader::loadWindow(
    const rapidjson::Value& doc
) noexcept{
    auto it=doc.FindMember("window");
    if(it==doc.MemberEnd() or not it->value.IsObject()){
        return std::nullopt;
    }

    auto window=it->value.GetObject();
    Blueprint::Window result={};

    // load title
    auto title=JsonHelper::getString(window, "title");
    if(title.has_value()){
        result.title=title.value();
    } else{
        logger.info("title not exist or type incorrect");
    }

    // load window position
    auto position=CustomHelper::getVector2D(window, "position");
    if(position.has_value()){
        auto _position=position.value();
        result.screen.x=_position.x;
        result.screen.y=_position.y;
    } else{
        logger.info("position not exist or type incorrect");
    }

    // load window size
    auto size=CustomHelper::getVector2D(window, "size");
    if(size.has_value()){
        auto _size=size.value();
        result.screen.width=_size.x;
        result.screen.height=_size.y;
    } else{
        logger.info("size not exist or type incorrect");
    }

    return result;
}
