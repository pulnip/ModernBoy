#pragma once

#include <memory>

#include "gefwd.hpp"

class IGameEngine{
  public:
    virtual ~IGameEngine()=default;

  protected:
    IGameEngine() noexcept=default;

  public:
    virtual void run() noexcept=0;
    virtual std::shared_ptr<ISubEngine> find(const SubEngineName name) noexcept=0;
};