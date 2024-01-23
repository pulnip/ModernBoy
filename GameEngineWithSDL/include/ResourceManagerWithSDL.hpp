#pragma once

#include <map>
#include <optional>
#include <string>

#include <SDL2/SDL_render.h>

#include "SubEngine/ResourceManager.hpp"

class ResourceManagerWithSDL final: public ResourceManager {
    friend class Game;

  public:
    ResourceManagerWithSDL() noexcept=default;
    ~ResourceManagerWithSDL();

    std::optional<SDL_Texture *> getTexture(const std::string &fileName) noexcept;

  private:
    void setAttribute() noexcept override final;

    // 이미지 로딩 과정 캡슐화
    SDL_Texture *loadTexture(const std::string &fileName) noexcept;

  private:
    std::weak_ptr<SDL_Renderer*> context;
    std::map<std::string, SDL_Texture*> textures;
};
