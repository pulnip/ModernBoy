#ifndef MODERNBOY_GAME_PHYSICS_SYSTEM_HPP
#define MODERNBOY_GAME_PHYSICS_SYSTEM_HPP

#include "fwd.hpp"
#include "interface.hpp"
#include "util/generator.hpp"
#include "game/game_fwd.hpp"

namespace ModernBoy::Game
{
    using SphereColliderProxy = std::tuple<EntityID, ArchetypeBit, TransformComponent*, RigidbodyComponent*, SphereColliderComponent*>;
    using SphereCollisionCandidate = std::pair<SphereColliderProxy, SphereColliderProxy>;

    class PhysicsSystem{
    public:
        PhysicsSystem(EntityRegistry& registry);

        Generator<void> update(DeltaTime dt);

    private:
        std::vector<SphereColliderProxy> flatten();
        std::vector<SphereCollisionCandidate> findCandidate();

        EntityRegistry& registry;
    };
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_PHYSICS_SYSTEM_HPP