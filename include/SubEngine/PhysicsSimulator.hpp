#pragma once

#include <map>

#include "SubEngine.hpp"

class PhysicsSimulator: public SubEngine,
    public Observer<Attribute_2D>
{
  private:
    using wpm=std::weak_ptr<MoveComponent>;

  public:
    virtual ~PhysicsSimulator()=default;

    void setCollision(
        std::weak_ptr<IActor> who, std::weak_ptr<IActor> to
    ) noexcept;

  protected:
    PhysicsSimulator() noexcept=default;

    struct AABB_model{
        AABB_model(const wpm& mc) noexcept;
        Vector2 position, velocity, size;
        
    };
    // first value: check collision (in dt)
    // second value: valid if collide
    // - first value: estimated collision time(seconds)
    // - second value: true if x-axis collision
    std::pair<bool, std::pair<float, bool>> AABB(
        const AABB_model& center, const AABB_model& opponent,
        const float& deltaTime
    ) noexcept;
    // first value: estimated collision time(seconds)
    // second value: true if x-axis collision
    std::pair<Math::Real, bool> whenCollide(
        const AABB_model& lhs, const AABB_model& rhs
    ) noexcept;
    bool isGetCloser(
        const AABB_model& center, const AABB_model& target
    ) noexcept;
    void redoWithCollisionAndFlipRelativeVelocity(
        const float& totalTime, const float& collideTime,
        Attribute_2D& attr, const Vector2& opponent,
        bool flipX
    ) noexcept;

  private:
    void injectDependency() noexcept override final{}
    SubEngineName getName() const noexcept override{
        return SubEngineName::PhysicsSimulator;
    }

  private:
    virtual void update(const float &deltaTime) noexcept override;

    virtual void onNotify(Attribute_2D attr) noexcept override=0;

  private:
    std::map<wpm, std::list<wpm>, std::owner_less<wpm>> collisionMap;
};

class NullPhysicsSimulator final: public PhysicsSimulator{
  private:
    void update(const float &deltaTime) noexcept override final{}
    void onNotify(Attribute_2D attr) noexcept override final{}
};

// for Leges motus Newtoni
class PhysicsSimulator_default final: public PhysicsSimulator{
  private:
    void update(const float &deltaTime) noexcept override final;
    void onNotify(Attribute_2D attr) noexcept override final;
};
