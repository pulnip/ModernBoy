#ifndef MODERNBOY_GAME_PHYSICS_SYSTEM_HPP
#define MODERNBOY_GAME_PHYSICS_SYSTEM_HPP

#include "fwd.hpp"
#include "interface.hpp"
#include "thread/generator.hpp"
#include "game_fwd.hpp"

namespace ModernBoy::Game
{
    class RaycastSystem{
    public:
        RaycastSystem(EntityRegistry&,
            RayService&,
            ModernBoy::Interface::DrawService&
        );
    
        void update(DeltaTime);

    private:
        EntityRegistry& registry;

        RayService& rayService;
        ModernBoy::Interface::DrawService& debugDrawService;
    };


    template<typename T>
    using ColliderProxy = std::tuple<EntityID, ArchetypeBit, Transform*, Rigidbody*, T*>;
    template<typename T>
    using CollisionCandidate = std::pair<ColliderProxy<T>, ColliderProxy<T>>;

    using SphereColliderProxy = ColliderProxy<SphereCollider>;
    using BoxColliderProxy = ColliderProxy<BoxCollider>;
    using SphereColliderCandidate = CollisionCandidate<SphereCollider>;
    using BoxColliderCandidate = CollisionCandidate<BoxCollider>;

    class PhysicsSystem{
    public:
        PhysicsSystem(EntityRegistry&,
            RayService&,
            ModernBoy::Interface::DrawService&
        );

        void update(DeltaTime);

    private:
        void simulateGravity(DeltaTime);
        void sphere2sphereCollision();
        void box2boxCollision();

        std::vector<SphereColliderProxy> getSphereColliderProxies();
        std::vector<BoxColliderProxy> getBoxColliderProxies();

        EntityRegistry& registry;
        RaycastSystem raycast;
    };
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_PHYSICS_SYSTEM_HPP