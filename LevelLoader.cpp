#include <fstream>
#include <vector>

#include "JsonHelper.hpp"
#include "Logger.hpp"
#include "MainEngine.hpp"

#include "LevelLoader.hpp"

bool LevelLoader::loadLevel(
    GameEngine* gameEngine,
    const std::string& fileName
) noexcept{
    auto doc=loadJson(fileName);
    if(not doc.has_value()){
        mainEngine.getLogger()->log("level", fileName, "load failed");
        return false;
    }

    // test code
    JsonHelper jsonHelper;
    auto [exist, opt]=jsonHelper.getInt(doc.value(), "version");
    if(not exist){
        mainEngine.getLogger()->log("attribute", "version", "not exist");
        return false;
    }
    if(not opt.has_value()){
        mainEngine.getLogger()->log("attribute", "version", "type incorrect");
        return false;
    }

    mainEngine.getLogger()->log("attribute", opt.value(), "found");
    return true;
}

std::optional<rapidjson::Document> LevelLoader::loadJson(
    const std::string& fileName
) noexcept{
    std::ifstream file(fileName,
        std::ios::in | std::ios::binary | std::ios::ate
    );
    if(not file.is_open()){
        mainEngine.getLogger()->log("file", fileName, "Not Found");
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
        mainEngine.getLogger()->log("file", fileName, "not valid json");
        return std::nullopt;
    }

    return doc;
}
