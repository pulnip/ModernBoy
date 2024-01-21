#pragma once

#include "Component/DrawComponent.hpp"
#include "Skin.hpp"

// 단일 스프라이트 텍스처
class SpriteComponent: public DrawComponent,
    public Observable<SpriteForSDL>
{
  public:
    virtual ~SpriteComponent()=default;

    void setTexture(SDL_Texture *t) noexcept{ texture=t; }

  protected:
    SpriteComponent() noexcept=default;

  public:
    virtual void draw() noexcept override;

  private:
    virtual ComponentName getName() const noexcept override{
        return ComponentName::SpriteComponent;
    }
    void injectDependency() noexcept override;
    virtual void update(const float& deltaTime) noexcept override{}

    int initDrawOrder() const noexcept override final{ return 201; }

  private:
    SDL_Texture *texture;
};
