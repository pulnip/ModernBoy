#pragma once

#include <map>
#include <optional>
#include <string>

#include "ResourceManager.hpp"

class SDL_Texture;

class ResourceManagerWithSDL : public ResourceManager {
    friend class Game;

  public:
    ~ResourceManagerWithSDL();

    std::optional<SDL_Texture *> getTexture(const std::string &fileName) noexcept;

  protected:
    ResourceManagerWithSDL(const OwnerRef owner) noexcept : ResourceManager(owner) {}

  private:
    void postConstruct() noexcept override;

    // 이미지 로딩 과정 캡슐화
    SDL_Texture *loadTexture(const std::string &fileName) noexcept;
    void setContext(const std::shared_ptr<SDL_Renderer *> context) noexcept {
        this->context = context;
    }

  private:
    std::weak_ptr<SDL_Renderer *> context;
    std::map<const std::string, SDL_Texture *> textures;
};
