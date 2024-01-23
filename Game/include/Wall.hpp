#pragma once

#include "Actor/Actor.hpp"

class Wall: public Actor{
  public:
    Wall() noexcept=default;
    virtual ~Wall()=default;

  protected:
    virtual void injectDependency() noexcept override;

  private:
    void updateActor(const float&) noexcept override final{}
};

class Ceil final: public Wall{
  public:
    Ceil() noexcept=default;
    ~Ceil()=default;

  private:
    void injectDependency() noexcept override final;
};

class Floor final: public Wall{
  public:
    Floor() noexcept=default;
    ~Floor()=default;

  private:
    void injectDependency() noexcept override final;
};

class RightWall final: public Wall{
  public:
    RightWall() noexcept=default;
    ~RightWall()=default;

  private:
    void injectDependency() noexcept override final;
};