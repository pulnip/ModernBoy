#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "SubEngine.hpp"

std::optional<SDL_Texture *>
SDL_ResourceManager::getSkin(const std::string &fileName) noexcept {
    auto [it, skinNotLoaded] = skins.try_emplace(fileName, nullptr);
    auto &texture = it->second;

    if (skinNotLoaded) {
        texture = loadTexture(fileName);
    }
    // skin file itself not exist.
    if (texture == nullptr) {
        return std::nullopt;
    }
    return texture;
}

SDL_Texture *SDL_ResourceManager::loadTexture(const std::string &fileName) noexcept {
    // 파일로부터 로딩
    SDL_Surface *surf = IMG_Load(fileName.c_str());
    if (!surf) {
        SDL_Log("Failed to load texture file %s", fileName);
        return nullptr;
    }

    // 텍스쳐 생성
    auto texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (!texture) {
        SDL_Log("Failed to convert surface to texture for %s", fileName);
        return nullptr;
    }
    return texture;
}
