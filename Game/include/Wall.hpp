#pragma once

#include "Actor/Actor.hpp"

class Wall: public Actor{
  public:
    virtual ~Wall()=default;

  protected:
    Wall() noexcept=default;

    virtual void injectDependency() noexcept override;

  private:
    void updateActor(const float&) noexcept override final{}
};

class Ceil: public Wall{
  private:
    void injectDependency() noexcept override final;
};

class Floor: public Wall{
  private:
    void injectDependency() noexcept override final;
};

class RightWall: public Wall{
  private:
    void injectDependency() noexcept override final;
};