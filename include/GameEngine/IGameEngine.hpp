#pragma once

#include <memory>

#include "Makable.hpp"
#include "Observer.hpp"
#include "gefwd.hpp"

class IGameEngine:
    public Makable<IGameEngine>,
    public Observer<Lifetime, ISubEngine>
{
  public:
    virtual ~IGameEngine()=default;

  protected:
    IGameEngine() noexcept=default;

  public:
    virtual void run() noexcept=0;
    virtual std::shared_ptr<ISubEngine> find(const SubEngineName name) noexcept=0;
};