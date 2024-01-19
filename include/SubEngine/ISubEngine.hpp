#pragma once

#include "Makable.hpp"
#include "gefwd.hpp"

class ISubEngine:
    public Makable<ISubEngine, IGameEngine>
{
  public:
    virtual ~ISubEngine()=default;

  protected:
    ISubEngine() noexcept=default;

  public:
    virtual SubEngineName getName() const noexcept=0;
    virtual void update(const float& deltaTime) noexcept=0;
};