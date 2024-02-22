#include <fstream>
#include <vector>
#include "Engine/Logger.hpp"
#include "LevelLoader.hpp"
#include "JsonHelper.hpp"

using namespace Engine;

LevelLoader::LevelLoader() noexcept:
    logger(std::make_unique<BindedLogger>("LevelLoader", "Base"))
{
    logger->debug("constructed");
}

LevelLoader::~LevelLoader(){
    logger->debug("destructed");
}

bool LevelLoader::loadLevel(
    const std::string& fileName
) noexcept{
    auto doc=loadJson(fileName);
    if(not doc.has_value()){
        logger->info("load failed");
        return false;
    }

    // test code
    JsonHelper jsonHelper;
    auto [exist, opt]=jsonHelper.getInt(doc.value(), "version");
    if(not exist){
        logger->info("attribute version not exist");
        return false;
    }
    if(not opt.has_value()){
        logger->info("attribute version type incorrect");
        return false;
    }

    logger->debug(("attribute"+ std::to_string(opt.value()) +"found").c_str());
    return true;
}

std::optional<rapidjson::Document> LevelLoader::loadJson(
    const std::string& fileName
) noexcept{
    std::ifstream file(fileName,
        std::ios::in | std::ios::binary | std::ios::ate
    );
    if(not file.is_open()){
        logger->info(("file"+fileName+"Not Found").c_str());
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
        logger->info(("file"+fileName+"not valid json").c_str());
        return std::nullopt;
    }

    return doc;
}
