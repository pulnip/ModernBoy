#ifndef MODERNBOY_GAME_PHYSICS_SYSTEM_HPP
#define MODERNBOY_GAME_PHYSICS_SYSTEM_HPP

#include "fwd.hpp"
#include "interface.hpp"
#include "util/generator.hpp"
#include "game/game_fwd.hpp"

namespace ModernBoy::Game
{
    using SphereColliderProxy = std::tuple<EntityID, ArchetypeBit, Transform*, Rigidbody*, SphereCollider*>;
    using SphereCollisionCandidate = std::pair<SphereColliderProxy, SphereColliderProxy>;

    class PhysicsSystem{
    public:
        PhysicsSystem(EntityRegistry& registry);

        void update(DeltaTime);
        void simulateGravity(DeltaTime);

    private:
        std::vector<SphereColliderProxy> asVector();

        EntityRegistry& registry;
    };
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_PHYSICS_SYSTEM_HPP