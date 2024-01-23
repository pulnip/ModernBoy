#pragma once

#include "Property.hpp"
#include "DrawComponent.hpp"

// Color Box 텍스처
class BoxComponent final: public DrawComponent,
    public UniqueObservable<ColorRect>
{
  public:
    ~BoxComponent()=default;
    BoxComponent() noexcept=default;

    void setTexture(const TrueColor &color) noexcept;

  private:
    void update(const float& deltaTime) noexcept override final{}
    void setAttribute() noexcept override final;

    void draw() noexcept override final;
    int initDrawOrder() const noexcept override final{ return 200; }

  private:
    Property<TrueColor> color;
};
