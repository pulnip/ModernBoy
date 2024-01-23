#pragma once

#include "Ownership.hpp"
#include "gefwd.hpp"
#include "ISubEngine.hpp"

class SubEngine: public ISubEngine,
    public Owned<SubEngine, GameEngine>
{
  public:
    virtual ~SubEngine()=default;

  protected:
    SubEngine() noexcept=default;

  private:
    void setProperty() noexcept override final{ injectDependency(); }

  private:
    virtual SubEngineName getName() const noexcept override{
        return SubEngineName::SubEngine;
    }
    virtual void update(const float& deltaTime) noexcept override=0;

    virtual void injectDependency() noexcept=0;
};
