#pragma once

#include "Component.hpp"
#include "Skin.hpp"

class DrawComponent : public Component {
  public:
    virtual void update(const float &deltaTime) noexcept override {}
    virtual void draw() = 0;

    virtual ComponentName getName() const noexcept override {
        return ComponentName::DrawComponent;
    }
    int getDrawOrder() const noexcept { return drawOrder; }

  protected:
    // drawOrder이 작을수록 더 뒤에 위치
    // 배경 계열: 100 to 199
    // 일반 오브젝트 계열: 200 to 299
    // player계열: 300 to 399
    DrawComponent(const std::weak_ptr<Actor> owner) noexcept
        : Component(owner) {
        updateOrder = 300;
    }

  private:
    void postConstruct() noexcept override;

  protected:
    // 그리기 순서(화가 알고리즘)
    int drawOrder = 0;
};
