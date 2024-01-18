#pragma once

#include "SubEngine.hpp"

class ResourceManager : public SubEngine {
  public:
    virtual ~ResourceManager()=default;

  protected:
    ResourceManager() noexcept=default;

  private:
    void update(const float &deltaTime) noexcept override {}
    SubEngineName getName() const noexcept override{
        return SubEngineName::ResourceManager;
    }

  private:
    virtual void injectDependency() noexcept override=0;
};

class NullResourceManager: public ResourceManager{
  private:
    void injectDependency() noexcept override final{}
};
