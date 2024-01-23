#pragma once

#include "Component.hpp"

class DrawComponent: public Component{
  public:
    virtual ~DrawComponent()=default;

    int getDrawOrder() const noexcept{ return drawOrder; }

  protected:
    DrawComponent() noexcept=default;


  private:
    ComponentName getName() const noexcept override final{
        return ComponentName::DrawComponent;
    }
    int initUpdateOrder() const noexcept override final{ return 300; }

  public:
    // 실제 화면 상에 그리기 위해서, GraphicsEngine에 이미지 전달.
    virtual void draw()=0;

  protected:
    virtual void injectDependency() noexcept override final;

  private:
    // 애니메이션처럼 내부 상태가 변해야할 경우 사용.
    virtual void update(const float &deltaTime) noexcept override=0;

    virtual int initDrawOrder() const noexcept=0;
    virtual void setAttribute() noexcept=0;

  protected:
    std::weak_ptr<MoveComponent> target;
    std::weak_ptr<GraphicsEngine> graphicsEngine;
    /* 그리기 순서(화가 알고리즘 *drawOrder이 작을수록 더 뒤에 위치)
    - 배경 계열: 100 to 199
    - 일반 오브젝트 계열: 200 to 299
    - player계열: 300 to 399
    */
    int drawOrder = 0;
};
