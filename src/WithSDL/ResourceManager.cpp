#include <cassert>
#include <SDL2/SDL_image.h>
#include "Engine/Core.hpp"
#include "Engine/Logger.hpp"
#include "WithSDL/Graphics.hpp"
#include "WithSDL/ResourceManager.hpp"

using namespace WithSDL;

constexpr bool HW_RENDERING = false;

ResourceManager::ResourceManager() noexcept:
    logger(std::make_unique<Engine::BindedLogger>("ResourceManager", "SDL")),
    context(static_cast<SDL_Renderer*>(Graphics::get()->context()))
{
    logger->debug("constructed");
}

ResourceManager::~ResourceManager(){
    logger->debug("destructed");
}

std::optional<SDL_Texture*>
ResourceManager::getTexture(const std::string& fileName) noexcept{
    if(context==nullptr){
        logger->info("context not valid, try to load context...");
        context=static_cast<SDL_Renderer*>(Graphics::get()->context());
        if(context==nullptr){
            logger->critical("context still not valid");
            return std::nullopt;
        }
    }

    auto [it, skinNotLoaded]=textures.try_emplace(
        fileName, nullptr
    );
    auto& texture = it->second;
    static const std::string path="C:/Users/choiw/Documents/GameEngineDevelopment/resource/";

    if(skinNotLoaded){
        texture = loadTexture(path + fileName);
    }

    // skin file itself not exist.
    if(texture == nullptr){
        return std::nullopt;
    }
    return texture;
}


SDL_Texture* ResourceManager::loadTexture(
    const std::string& fileName
) noexcept{
    if(context==nullptr){
        return nullptr;
    }

    SDL_Texture* texture=nullptr;
    // 파일로부터 로딩
    if (HW_RENDERING) {
        texture = IMG_LoadTexture(context, fileName.c_str());
        if (texture == nullptr) {
            SDL_Log("Failed to Load Texture from File: %s", fileName.c_str());
        }
    } else {
        SDL_Surface *surface = IMG_Load(fileName.c_str());
        if (surface == nullptr) {
            SDL_Log("Failed to Load Surface from File: %s", fileName.c_str());
            return nullptr;
        }

        texture = SDL_CreateTextureFromSurface(context, surface);
        if (!texture) {
            SDL_Log("Failed to Create Texture from Surface: %s", SDL_GetError());
        }
        SDL_FreeSurface(surface);
    }

    return texture;
}
