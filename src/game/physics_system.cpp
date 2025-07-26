#include "physics.hpp"
#include "game/context.hpp"
#include "game/physics_system.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

PhysicsSystem::PhysicsSystem(EntityRegistry& registry)
:registry(registry){}

void PhysicsSystem::update(DeltaTime dt){
    auto dt_ = dt.count() / 1'000'000.0f;

    for(auto [id, bit, tc, rc]: registry.query<TransformComponent, RigidbodyComponent>()){
        tc.position += rc.velocity * dt_;
    }

    auto collidedCandidate = findCandidate();
    std::vector<EntityID> collided;
    auto notCollidedCandidate = findCollidedBefore();

    for(auto [src, tgt]: collidedCandidate){
        auto [src_id, src_bit, src_tc, src_rc, src_sc] = src;
        auto [tgt_id, tgt_bit, tgt_tc, tgt_rc, tgt_sc] = tgt;

        auto src_pos = src_tc->position + tgt_tc->position;
        auto tgt_pos = tgt_tc->position + tgt_tc->position;

        if(sphereCollision(src_pos, src_sc->radius, tgt_pos, tgt_sc->radius)){
            if((src_bit & COLLIDED_BIT) == 0){
                std::erase(notCollidedCandidate, src_id);
                collided.emplace_back(src_id);
            }
            if((tgt_bit & COLLIDED_BIT) == 0){
                std::erase(notCollidedCandidate, tgt_id);
                collided.emplace_back(tgt_id);
            }
        }
    }

    for(auto id: collided){
        registry.appendComponent(id, Collided{ .entity=id });
    }
    for(auto id: notCollidedCandidate){
        registry.removeComponent<Collided>(id);
    }
}

// Generator<void> PhysicsSystem::update(DeltaTime dt){
//     auto dt_ = dt.count() / 1'000'000.0f;

//     for(auto [id, bit, tc, rc]: registry.query<TransformComponent, RigidbodyComponent>()){
//         tc.position += rc.velocity * dt_;

//         co_yield 0;
//     }

//     auto collidedCandidate = findCandidate();
//     std::vector<EntityID> collided;
//     auto notCollidedCandidate = findCollidedBefore();

//     for(auto [src, tgt]: collidedCandidate){
//         auto [src_id, src_bit, src_tc, src_rc, src_sc] = src;
//         auto [tgt_id, tgt_bit, tgt_tc, tgt_rc, tgt_sc] = tgt;

//         auto src_pos = src_tc->position + tgt_tc->position;
//         auto tgt_pos = tgt_tc->position + tgt_tc->position;

//         if(sphereCollision(src_pos, src_sc->radius, tgt_pos, tgt_sc->radius)){
//             if((src_bit & COLLIDED_BIT) == 0){
//                 std::erase(notCollidedCandidate, src_id);
//                 collided.emplace_back(src_id);
//             }
//             if((tgt_bit & COLLIDED_BIT) == 0){
//                 std::erase(notCollidedCandidate, tgt_id);
//                 collided.emplace_back(tgt_id);
//             }
//         }
//     }

//     for(auto id: collided){
//         registry.appendComponent(id, Collided{ .entity=id });
//     }
//     for(auto id: notCollidedCandidate){
//         registry.removeComponent<Collided>(id);
//     }

//     co_return;
// }

std::vector<SphereColliderProxy> PhysicsSystem::flatten(){
    std::vector<SphereColliderProxy> flat;

    for(auto [id, bit, tc, rc, sc]: registry.query<TransformComponent, RigidbodyComponent, SphereColliderComponent>()){
        flat.emplace_back(id, bit, &tc, &rc, &sc);
    }

    return flat;
}
std::vector<SphereCollisionCandidate> PhysicsSystem::findCandidate(){
    std::vector<SphereColliderProxy> proxies = flatten();

    std::vector<SphereCollisionCandidate> candidates;
    for(Index i=0; i<proxies.size(); ++i){
        auto [src_id, src_bit, src_tc, src_rc, src_sc] = proxies[i];

        for(Index j=i+1; j<proxies.size(); ++j){
            auto [tgt_id, tgt_bit, tgt_tc, tgt_rc, tgt_sc] = proxies[j];

            auto tgt_to_src = src_tc->position - tgt_tc->position;
            auto rel_vel = tgt_rc->velocity - src_rc->velocity;

            if(dot(tgt_to_src, rel_vel) > 0)
                candidates.emplace_back(proxies[i], proxies[j]);
        }
    }

    return candidates;
}

std::vector<EntityID> PhysicsSystem::findCollidedBefore(){
    std::vector<EntityID> tagged;

    for(auto [id, bit, _]: registry.query<Collided>()){
        tagged.emplace_back(id);
    }

    return tagged;
}