#pragma once

#include "Math.hpp"
#include "Skin.hpp"
#include "DrawComponent.hpp"

// Color Box 텍스처
class BoxComponent : public DrawComponent, public Observable<ColorRect> {
  public:
    void update(const float& deltaTime) noexcept override final{}
    void draw() noexcept override final;
    ComponentName getName() const noexcept override final{
        return ComponentName::BoxComponent;
    }

    void setTexture(const TrueColor &color, const Vector2 &size) noexcept;

  protected:
    BoxComponent() noexcept{
        drawOrder = 200;
    }

  private:
    void injectDependency() noexcept override final;

  private:
    TrueColor color;
    Vector2 boxSize;
};
