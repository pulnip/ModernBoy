#pragma once

#include <list>
#include <map>
#include <utility>
#include <vector>

#include "Math.hpp"
#include "Observer.hpp"
#include "gefwd.hpp"

#include "SubEngine.hpp"

namespace Game{
    namespace SubEngine{
        class PhysicsSimulator final: public Interface,
            public Observer<Skin::Attribute_2D>
        {
        private:
            using ptr=std::weak_ptr<Component::Movable>;

        public:
            PhysicsSimulator() noexcept=default;
            virtual ~PhysicsSimulator()=default;

            virtual void update(const Time& deltaTime) noexcept override;

            Type getType() const noexcept override final{
                return Type::PhysicsSimulator;
            }
            void appendMovable(ptr movable) noexcept;
            void setCollision(
                std::weak_ptr<Actor::Interface> who, std::weak_ptr<Actor::Interface> to
            ) noexcept;

        protected:
            struct AABB_model{
                AABB_model(const ptr& mc) noexcept;
                Vector2D position, velocity, size;
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
            std::pair<My::Math::Number::Real, CollisionHint> whenCollide(
                const AABB_model& lhs, const AABB_model& rhs
            ) noexcept;
            bool isGetCloser(
                const AABB_model& center, const AABB_model& target
            ) noexcept;
            void redoWithCollisionAndFlipRelativeVelocity(
                const Time& totalTime, const Time& collideTime,
                Skin::Attribute_2D& attr, const Vector2D& opponent,
                const CollisionHint& hint
            ) noexcept;

        private:
            std::map<
                ptr, // movable
                std::list<ptr>, // collide opponent
                std::owner_less<ptr>
            > collisionMap;
        };
    }
}