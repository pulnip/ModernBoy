#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>
#include <SDL2/SDL_render.h>
#include "myfwd.hpp"
#include "Engine/Logger.hpp"
#include "Engine/ResourceManager.hpp"

namespace WithSDL{
    class ResourceManager final: public Engine::ResourceManager{
        friend class Core;
    public:
        ResourceManager() noexcept;
        static void make() noexcept{
            ::Engine::ResourceManager::make<ResourceManager>();
        }


        std::optional<SDL_Texture*> getTexture(
            const std::string &fileName
        ) noexcept;
        std::optional<std::vector<SDL_Texture*>> getTexture(
            const std::vector<std::string>& fileNames
        ) noexcept;

    private:
        // 이미지 로딩 과정 캡슐화
        SDL_Texture* loadTexture(const std::string& fileName) noexcept;
        SDL_Texture* sw_render(const std::string& fileName) noexcept;
        SDL_Texture* hw_render(const std::string& fileName) noexcept;

    private:
        ::Logger::Binded logger={"Resource", "SDL"};
        SDL_Renderer* context=nullptr;
        std::map<std::string, SDL_Texture*> textures;
    };
}
