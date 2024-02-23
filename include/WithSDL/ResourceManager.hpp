#pragma once

#include <map>
#include <optional>
#include <string>
#include <SDL2/SDL_render.h>
#include "myfwd.hpp"
#include "Engine/ResourceManager.hpp"

namespace WithSDL{
    class ResourceManager final: Engine::ResourceManager{
        friend class Core;
    public:
        ResourceManager() noexcept;
        ~ResourceManager();

        std::optional<SDL_Texture*> getTexture(
            const std::string &fileName
        ) noexcept;

    private:
        // 이미지 로딩 과정 캡슐화
        SDL_Texture* loadTexture(const std::string &fileName) noexcept;

    private:
        std::unique_ptr<Engine::BindedLogger> logger;
        SDL_Renderer* context=nullptr;
        std::map<std::string, SDL_Texture*> textures;
    };
}
