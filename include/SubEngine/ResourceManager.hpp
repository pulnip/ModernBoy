#pragma once

#include "SubEngine.hpp"

class ResourceManager : public SubEngine {
  public:
    virtual ~ResourceManager()=default;

  protected:
    ResourceManager() noexcept=default;

  private:
    void injectDependency() noexcept override final{ setAttribute(); }

  private:
    SubEngineName getName() const noexcept override{
        return SubEngineName::ResourceManager;
    }
    void update(const float &deltaTime) noexcept override {}
    virtual void setAttribute() noexcept=0;
};

class NullResourceManager final: public ResourceManager{
  private:
    void setAttribute() noexcept override final{}
};
