#pragma once

#include <memory>
#include <set>

#include "Observer.hpp"
#include "Skin.hpp"
#include "SubEngine.hpp"

class DrawComponent;

class GraphicsEngine : public SubEngine, public Observer<ColorRect> {
  private:
    using Drawable = std::shared_ptr<DrawComponent>;
    struct DrawOrder {
        bool operator()(const Drawable &lhs, const Drawable &rhs) const;
    };

  public:
    virtual ~GraphicsEngine() = default;
    void update(const float &deltaTime) noexcept override;

  protected:
    GraphicsEngine(const OwnerRef owner) noexcept : SubEngine(owner) {}

  private:
    virtual void postConstruct() noexcept override = 0;
    virtual void onNotify(ColorRect rect) noexcept override = 0;

    virtual void prepareRendering() noexcept = 0;
    virtual void finalizeRendering() noexcept = 0;

  private:
    // ordered by Draw Order(Draw Component's);
    std::multiset<Drawable, DrawOrder> drawables;
};
