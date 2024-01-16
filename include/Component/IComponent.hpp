#pragma once

#include "gefwd.hpp"

class IComponent{
  public:
    virtual ~IComponent()=default;

    virtual void update(const float& deltaTime) noexcept=0;

    virtual ComponentName getName() const noexcept=0;
    virtual int getUpdateOrder() const noexcept=0;
};