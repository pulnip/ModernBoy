#pragma once

#include <memory>
#include "FWD.hpp"
#include "Component/Component.hpp"
#include "Component/Movable/Types.hpp"

class Drawable: public Component{
  protected:
    using Pos=Transform::Position;

  public:
    Drawable(int drawOrder) noexcept:
        drawOrder(drawOrder), Component(1000){}
    virtual ~Drawable();

    void update(const Time&, Actor&) noexcept override;

    int getDrawOrder() const noexcept{ return drawOrder; }
    Type getType() noexcept override{
        return Type::Drawable;
    }

  private:
    virtual void drawAt(const Pos&) noexcept=0;

  private:
    Logging::Bind logger={"Drawable", "base"};
    /* 그리기 순서(화가 알고리즘 *drawOrder이 작을수록 더 뒤에 위치)
    - 배경 계열: 100 to 199
    - 일반 오브젝트 계열: 200 to 299
    - player계열: 300 to 399
    */
    int drawOrder;
};
