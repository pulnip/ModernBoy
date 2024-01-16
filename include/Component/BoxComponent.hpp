#pragma once

#include "DrawComponent.hpp"
#include "Math.hpp"

// Color Box 텍스처
class BoxComponent : public DrawComponent, public Observable<ColorRect> {
  public:
    void draw() noexcept override;

    ComponentName getName() const noexcept override {
        return ComponentName::BoxComponent;
    }
    void setTexture(const TrueColor &color, const Vector2 &size) noexcept;

  protected:
    BoxComponent() noexcept{
        drawOrder = 200;
    }

    TrueColor color;
};
