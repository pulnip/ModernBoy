#pragma once

#include <memory>

#include "Observer.hpp"
#include "gefwd.hpp"

class ISubEngine{
  public:
    virtual ~ISubEngine()=default;

    virtual SubEngineName getName() const noexcept=0;
    virtual void update(const float& deltaTime) noexcept=0;

  protected:
    ISubEngine() noexcept=default;
};