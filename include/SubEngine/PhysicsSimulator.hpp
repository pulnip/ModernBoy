#pragma once

#include <utility>
#include <map>
#include <vector>

#include "Math.hpp"
#include "SubEngine.hpp"

class PhysicsSimulator final: public SubEngine,
    public Observer<Attribute_2D>
{
  private:
    using wpm=std::weak_ptr<MoveComponent>;

  public:
    PhysicsSimulator() noexcept=default;
    virtual ~PhysicsSimulator()=default;

    void setCollision(
        std::weak_ptr<IActor> who, std::weak_ptr<IActor> to
    ) noexcept;
    void appendMovable(wpm movable) noexcept;

  protected:
    struct AABB_model{
        AABB_model(const wpm& mc) noexcept;
        Vector2 position, velocity, size;
    };
    using CollisionHint=std::pair<
        bool, // true if x-collision,
        bool // true if y-collision
    >;
    // first value: check collision (in dt)
    // second value: valid if collide
    // - first value: estimated collision time(seconds)
    // - second value: true if x-axis collision
    std::pair<bool, std::pair<float, CollisionHint>> AABB(
        const AABB_model& center, const AABB_model& opponent,
        const float& deltaTime
    ) noexcept;
    // first value: estimated collision time(seconds)
    // second value: true if x-axis collision
    std::pair<Math::Real, CollisionHint> whenCollide(
        const AABB_model& lhs, const AABB_model& rhs
    ) noexcept;
    bool isGetCloser(
        const AABB_model& center, const AABB_model& target
    ) noexcept;
    void redoWithCollisionAndFlipRelativeVelocity(
        const float& totalTime, const float& collideTime,
        Attribute_2D& attr, const Vector2& opponent,
        const CollisionHint& hint
    ) noexcept;

  private:
    SubEngineName getName() const noexcept override final{
        return SubEngineName::PhysicsSimulator;
    }

  private:
    virtual void injectDependency() noexcept override{}
    virtual void update(const float &deltaTime) noexcept override;
    virtual void onNotify(Attribute_2D attr) noexcept override;

  private:
    std::map<
        wpm, // movable
        std::list<wpm>, // collide opponent
        std::owner_less<wpm>
    > collisionMap;
};
