#include <SDL2/SDL_image.h>
#include <SDL2/SDL_log.h>

#include "ResourceManagerWithSDL.hpp"

ResourceManagerWithSDL::~ResourceManagerWithSDL() {
    for (auto &[file, texture] : textures) {
        SDL_DestroyTexture(texture);

        SDL_Log("Texture Unloaded: %s", file.c_str());
    }
}

std::optional<SDL_Texture *>
ResourceManagerWithSDL::getTexture(const std::string &fileName) noexcept {
    auto [it, skinNotLoaded] = textures.try_emplace(fileName, nullptr);
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

void ResourceManagerWithSDL::injectDependency() noexcept {
#warning "set Context?"
}

constexpr bool HW_RENDERING = false;

SDL_Texture*
ResourceManagerWithSDL::loadTexture(const std::string &fileName) noexcept {
    SDL_Texture *texture;

    if (context.expired()) {
        return nullptr;
    }
    auto renderer = context.lock();

    // 파일로부터 로딩
    if (HW_RENDERING) {
        texture = IMG_LoadTexture(*renderer, fileName.c_str());
        if (texture == nullptr) {
            SDL_Log("Failed to Load Texture from File: %s", fileName.c_str());
        }
    } else {
        SDL_Surface *surface = IMG_Load(fileName.c_str());
        if (surface == nullptr) {
            SDL_Log("Failed to Load Surface from File: %s", fileName.c_str());
            return nullptr;
        }

        texture = SDL_CreateTextureFromSurface(*renderer, surface);
        if (!texture) {
            SDL_Log("Failed to Create Texture from Surface: %s", SDL_GetError());
        }
        SDL_FreeSurface(surface);
    }

    return texture;
}

void ResourceManagerWithSDL::setContext(
    const std::shared_ptr<SDL_Renderer*> context) noexcept
{
    this->context = context;
}
