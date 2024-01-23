#pragma once

#include "Component/DrawComponent.hpp"
#include "Skin.hpp"

// 단일 스프라이트 텍스처
class SpriteComponent: public DrawComponent,
    public Observable<SpriteForSDL>
{
  public:
    virtual ~SpriteComponent()=default;
    SpriteComponent() noexcept=default;

    void setTexture(SDL_Texture *t) noexcept{ texture=t; }

  private:
    virtual void draw() noexcept override;
    virtual void update(const float& deltaTime) noexcept override{}
    virtual int initDrawOrder() const noexcept override{ return 201; }
    virtual void setAttribute() noexcept override;

  private:
    SDL_Texture *texture;
};
