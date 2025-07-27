#include "physics.hpp"
#include "game/context.hpp"
#include "game/physics_system.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

PhysicsSystem::PhysicsSystem(EntityRegistry& registry)
:registry(registry){}

void PhysicsSystem::update(DeltaTime dt){
    auto dt_ = dt.count() / 1'000'000.0f;

    auto vec = asVector();
    for(Index i=0; i<vec.size(); ++i){
        auto [src_id, src_bit, src_tc, src_rc, src_sc] = vec[i];
        auto src_pos = src_tc->position + src_sc->position;
        auto src_rad = src_sc->radius;

        for(Index j=i+1; j<vec.size(); ++j){
            auto [tgt_id, tgt_bit, tgt_tc, tgt_rc, tgt_sc] = vec[j];
            auto tgt_pos = tgt_tc->position + tgt_sc->position;
            auto tgt_rad = tgt_sc->radius;

            auto [_1, src_c_ret] = registry.query_safe<Collided>(src_id);
            auto [_2, tgt_c_ret] = registry.query_safe<Collided>(src_id);
            if(sphereCollision(src_pos, src_rad, tgt_pos, tgt_rad)){
                if(!src_c_ret){
                    registry.appendComponent(src_id, Collided{.entity=src_id, .isActive=true });
                    auto [src_mc, src_mc_ret] = registry.query_safe<Model>(src_id);
                    if(src_mc_ret)
                        src_mc.alpha = 0.5f;
                }
                if(!tgt_c_ret){
                    registry.appendComponent(tgt_id, Collided{.entity=tgt_id, .isActive=true });
                    auto [tgt_mc, tgt_mc_ret] = registry.query_safe<Model>(tgt_id);
                    if(tgt_mc_ret)
                        tgt_mc.alpha = 0.5f;
                }
            }
            else{
                if(src_c_ret){
                    registry.removeComponent<Collided>(src_id);
                    auto [src_mc, src_mc_ret] = registry.query_safe<Model>(src_id);
                    if(src_mc_ret)
                        src_mc.alpha = 1.0f;
                }
                if(tgt_c_ret){
                    registry.removeComponent<Collided>(tgt_id);
                    auto [tgt_mc, tgt_mc_ret] = registry.query_safe<Model>(tgt_id);
                    if(tgt_mc_ret)
                        tgt_mc.alpha = 1.0f;
                }
            }
        }
    }

    for(auto [id, bit, tc, rc]: registry.query<Transform, Rigidbody>()){
        tc.position += rc.velocity * dt_;
    }
}

std::vector<SphereColliderProxy> PhysicsSystem::asVector(){
    std::vector<SphereColliderProxy> flat;

    for(auto [id, bit, tc, rc, sc]: registry.query<Transform, Rigidbody, SphereCollider>()){
        flat.emplace_back(id, bit, &tc, &rc, &sc);
    }

    return flat;
}
