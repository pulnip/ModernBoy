#include <fstream>
#include <vector>
#include "Engine/LevelLoader.hpp"
#include "Engine/JsonHelper.hpp"

using namespace WithRapidjson;

bool LevelLoader::loadLevel(
    const std::string& fileName
) noexcept{
    auto doc=loadJson(fileName);
    if(not doc.has_value()){
        logger.info("load failed");
        return false;
    }

    // test code
    auto version=JsonHelper::getInt(doc.value(), "version");
    if(not version.has_value()){
        logger.info("attribute version not exist or type incorrect");
        return false;
    }

    logger.debug(std::to_string(version.value()).c_str());
    // test/

    return true;
}

std::optional<rapidjson::Document> LevelLoader::loadJson(
    const std::string& fileName
) noexcept{
    std::ifstream file(fileName,
        std::ios::in | std::ios::binary | std::ios::ate
    );
    if(not file.is_open()){
        logger.info(("file"+fileName+"Not Found").c_str());
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
