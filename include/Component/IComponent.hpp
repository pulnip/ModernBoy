#pragma once

#include "Makable.hpp"
#include "gefwd.hpp"

class IComponent:
    public Makable<IComponent, IActor>
{
  public:
    virtual ~IComponent()=default;

  protected:
    IComponent() noexcept=default;

  public:
    virtual ComponentName getName() const noexcept=0;
    virtual void update(const float& deltaTime) noexcept=0;
    const int& getUpdateOrder() const noexcept{ return updateOrder; }

  protected:
    /* 컴포넌트의 업데이트 순서
    - updateOrder이 작을수록 더 빨리 갱신
    - input 계열: 100 to 199
    - 연산 계열: 200 to 299
    - output계열: 300 to 399
    */
    int updateOrder=0;
};