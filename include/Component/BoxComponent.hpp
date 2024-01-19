#pragma once

#include "Math.hpp"
#include "Skin.hpp"
#include "DrawComponent.hpp"

// Color Box 텍스처
class BoxComponent: public DrawComponent,
    public Observable<ColorRect>
{
  public:
    ~BoxComponent()=default;

    void setTexture(const TrueColor &color, const Vector2 &size) noexcept;

  protected:
    BoxComponent() noexcept=default;

  public:
    void draw() noexcept override final;

  private:
    void injectDependency() noexcept override final;
    void update(const float& deltaTime) noexcept override final{}

    int initDrawOrder() const noexcept override final{ return 200; }

  private:
    TrueColor color;
    Vector2 boxSize;
};
