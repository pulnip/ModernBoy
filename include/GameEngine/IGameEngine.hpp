#pragma once

#include <memory>

#include "Observer.hpp"
#include "gefwd.hpp"

class IGameEngine: public Observer<Lifetime, SubEngine>{
  public:
    virtual ~IGameEngine()=default;

    virtual void run() noexcept=0;
    virtual std::shared_ptr<SubEngine> query(const SubEngineName name) noexcept=0;

  protected:
    IGameEngine() noexcept=default;
};