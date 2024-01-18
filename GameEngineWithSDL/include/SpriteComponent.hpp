#pragma once

#include "Component/DrawComponent.hpp"
#include "Skin.hpp"

// 단일 스프라이트 텍스처
class SpriteComponent : public DrawComponent, public Observable<SpriteForSDL> {
  public:
    virtual void update(const float& deltaTime) noexcept override{}
    virtual void draw() noexcept override;
    virtual ComponentName getName() const noexcept override {
        return ComponentName::SpriteComponent;
    }

    virtual void setTexture(SDL_Texture *texture) noexcept;

  protected:
    SpriteComponent() noexcept{
        drawOrder = 201;
    }

  private:
    void injectDependency() noexcept override;

    SDL_Texture *texture;
    Vector2 spriteSize;
};
