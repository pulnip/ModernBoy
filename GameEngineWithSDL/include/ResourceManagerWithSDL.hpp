#pragma once

#include <map>
#include <optional>
#include <string>

#include "SubEngine/ResourceManager.hpp"

class SDL_Texture;
class SDL_Renderer;

namespace WithSDL{
    namespace SubEngine{
        class ResourceManager final:
            public Game::SubEngine::ResourceManager
        {
        public:
            ResourceManager() noexcept=default;
            ~ResourceManager();

            std::optional<SDL_Texture*> getTexture(
                const std::string &fileName
            ) noexcept;

        private:
            void postConstruct() noexcept override final;
            // 이미지 로딩 과정 캡슐화
            SDL_Texture* loadTexture(const std::string &fileName) noexcept;

        private:
            SDL_Renderer* context=nullptr;
            std::map<std::string, SDL_Texture*> textures;
        };
    }
}
