#include <fstream>
#include <vector>
#include "Engine/Logger.hpp"
#include "LevelLoader.hpp"
#include "JsonHelper.hpp"

using namespace Engine;

LevelLoader::LevelLoader() noexcept:
    Binded("LevelLoader", "Base"){}

LevelLoader::~LevelLoader(){}

bool LevelLoader::loadLevel(
    const std::string& fileName
) noexcept{
    auto doc=loadJson(fileName);
    if(not doc.has_value()){
        info("load failed");
        return false;
    }

    // test code
    auto version=JsonHelper::getInt(doc.value(), "version");
    if(not version.has_value()){
        info("attribute version not exist or type incorrect");
        return false;
    }

    debug(
        ("attribute"+std::to_string(version.value())+"found").c_str());
    return true;
}

std::optional<rapidjson::Document> LevelLoader::loadJson(
    const std::string& fileName
) noexcept{
    std::ifstream file(fileName,
        std::ios::in | std::ios::binary | std::ios::ate
    );
    if(not file.is_open()){
        info(("file"+fileName+"Not Found").c_str());
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
        info(("file"+fileName+"not valid json").c_str());
        return std::nullopt;
    }

    return doc;
}
