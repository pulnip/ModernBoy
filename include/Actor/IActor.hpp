#pragma once

#include <memory>
#include <optional>

#include "gefwd.hpp"

class IActor{
  public:
    // 액터의 상태를 추적하는 데 사용
    enum class State{
        EActive,
        EPaused,
        EDead
    };

  public:
    virtual ~IActor() = default;

    const State& getState() const noexcept{ return state; }

  protected:
    IActor() noexcept=default;

  public:
    // ActorManager에서 호출.
    virtual void update(const float& deltaTime) noexcept = 0;
    virtual std::shared_ptr<IComponent>
    find(const ComponentName name) noexcept=0;
    virtual std::optional<std::shared_ptr<ISubEngine>>
    query(const SubEngineName name) noexcept=0;

  protected:
    State state=State::EActive;
};
