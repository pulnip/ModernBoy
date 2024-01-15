#pragma once

#include "Component/DrawComponent.hpp"

// 단일 스프라이트 텍스처
class SpriteComponent : public DrawComponent, public Observable<SDL_Sprite> {
  public:
    virtual void draw() noexcept override;

    virtual ComponentName getName() const noexcept override {
        return ComponentName::SpriteComponent;
    }
    virtual void setTexture(SDL_Texture *texture) noexcept;

  protected:
    SpriteComponent(const std::weak_ptr<Actor> owner) noexcept
        : DrawComponent(owner) {
        drawOrder = 201;
    }

    SDL_Texture *texture;
};
