#include <SDL2/SDL.h>

#include "SubEngine.hpp"

std::optional<SDL_Texture *>
SDL_ResourceManager::getTexture(const std::string &fileName) noexcept {
    auto result = textures.find(fileName);
    SDL_Texture *texture = nullptr;

    if (result == textures.end()) {
        const auto &[pair, result] = textures.emplace(fileName, loadTexture(fileName));
        const auto &[_, loaded] = *pair;
        return loaded;
    }
}

SDL_Texture *SDL_ResourceManager::loadTexture(const std::string &fileName) noexcept {
    // 파일로부터 로딩
    SDL_Surface *surf = IMG_Load(fileName);
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
