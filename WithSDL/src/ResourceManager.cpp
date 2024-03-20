#include <cassert>
#include <SDL2/SDL_image.h>
#include "Engine/Core.hpp"
#include "Engine/Logger.hpp"
#include "WithSDL/Graphics.hpp"
#include "WithSDL/ResourceManager.hpp"

using namespace WithSDL;

ResourceManager::ResourceManager() noexcept:
context(static_cast<SDL_Renderer*>(Graphics::get()->context())){}

std::optional<SDL_Texture*>
ResourceManager::getTexture(const std::string& fileName) noexcept{
    // find from cache
    auto [cached, notCached]=textures.try_emplace(
        fileName, nullptr
    );

    if(notCached){
        cached->second=loadTexture(fileName);
    }

    SDL_Texture* texture=cached->second;
    // skin file itself not exist.
    if(texture==nullptr){
        return std::nullopt;
    }
    return texture;
}

std::optional<std::vector<SDL_Texture*>> 
ResourceManager::getTexture(
    const std::vector<std::string>& fileNames
) noexcept{
    std::vector<SDL_Texture*> textures;
    for(const auto& fileName: fileNames){
        auto texture=getTexture(fileName);
        
        if(texture.has_value()){
            textures.emplace_back(texture.value());
        } else{
            return std::nullopt;
        }
    }
    return textures;
}

SDL_Texture* ResourceManager::loadTexture(
    const std::string& fileName
) noexcept{
    constexpr bool SW_RENDERING=true;
    static std::string path
        ="C:/Users/choiw/Documents/GameEngineDevelopment/resource/";
    const std::string fileNameWithPath=path+fileName;

    if(context==nullptr){
        context=static_cast<SDL_Renderer*>(Graphics::get()->context());
        if(context==nullptr){
            logger.info("context not valid");
            return nullptr;
        }
    }

    if(SW_RENDERING){
        return sw_render(fileNameWithPath);
    } else{
        return hw_render(fileNameWithPath);
    }
}


SDL_Texture* ResourceManager::sw_render(
    const std::string& fileNameWithPath
) noexcept{
    auto surface = IMG_Load(fileNameWithPath.c_str());
    if(surface == nullptr){
        logger.info("Failed to Load Surface from File");
        logger.info(fileNameWithPath.c_str());
        return nullptr;
    }

    auto texture=SDL_CreateTextureFromSurface(context, surface);
    if(texture==nullptr){
        logger.info("Failed to Create Texture from Surface");
        logger.info(SDL_GetError());
    }
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Texture* ResourceManager::hw_render(
    const std::string& fileNameWithPath
) noexcept{
    auto texture=IMG_LoadTexture(context, fileNameWithPath.c_str());
    if(texture==nullptr){
        logger.info("Failed to Load Texture from File");
        logger.info(SDL_GetError());
    }
    return texture;
}
