#pragma once

enum class ComponentName{
    Component,
    AIComponent,
    CollisionComponent,
    DrawComponent,
    InputComponent,
    MoveComponent
};

class IComponent{
  public:
    virtual ~IComponent()=default;

    const int& getUpdateOrder() const noexcept{ return updateOrder; }

  protected:
    IComponent() noexcept=default;

  public:
    virtual void update(const float& deltaTime) noexcept=0;
    virtual ComponentName getName() const noexcept=0;

  protected:
    /* 컴포넌트의 업데이트 순서
    - updateOrder이 작을수록 더 빨리 갱신
    - input 계열: 100 to 199
    - 연산 계열: 200 to 299
    - output계열: 300 to 399
    */
    int updateOrder=0;
};