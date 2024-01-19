#pragma once

#include <memory>
#include <optional>

#include "Makable.hpp"
#include "Observer.hpp"
#include "gefwd.hpp"

class IActor:
    public Makable<IActor, ActorManager>,
    public Observer<Lifetime, IComponent>
{
  public:
    // 액터의 상태를 추적하는 데 사용
    enum class State{
        EActive,
        EPaused,
        EDead
    };

  public:
    virtual ~IActor() = default;

  protected:
    IActor() noexcept=default;

  public:
    // ActorManager에서 호출.
    virtual void update(const float& deltaTime) noexcept = 0;
    virtual std::shared_ptr<IComponent>
    find(const ComponentName name) noexcept=0;
    virtual std::optional<std::shared_ptr<ISubEngine>>
    query(const SubEngineName name) noexcept=0;

    const State& getState() const noexcept{ return state; }

  protected:
    State state=State::EActive;
};
