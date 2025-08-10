#include "control_system.hpp"
#include "entity_registry.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

ControlSystem::ControlSystem(EntityRegistry& registry,
    IntentService& playerSrv, IntentService& editorSrv)
:playerControlSystem(registry, playerSrv)
,editorControlSystem(registry, editorSrv){}

void ControlSystem::update(DeltaTime deltaTime){
    playerControlSystem.update(deltaTime);
    editorControlSystem.update(deltaTime);
}

PlayerControlSystem::PlayerControlSystem(
    EntityRegistry& registry, IntentService& service)
:registry(registry), service(service){}

void PlayerControlSystem::update(DeltaTime deltaTime){
    auto dt = deltaTime.count() / 1'000'000.0f;

    auto moveIntents = service.drainMoveIntents();
    auto lookIntents = service.drainLookIntents();

    for(auto [id, bit, tf, _1]: registry.query<Transform, Player>()){
        auto right_ground = ground_right(tf.rotation);
        // move.y might not be used
        auto forward_ground = ground_forward(tf.rotation);

        for(const auto& intent: moveIntents){
            auto dx = right_ground * dt * intent.move.x;
            auto dz = forward_ground * dt * intent.move.z;

            tf.position += dx + dz;
        }
        for(const auto& intent: lookIntents){
            auto qYaw   = axisAngle(unitY(), intent.yaw);
            auto qPitch = axisAngle(right(tf.rotation), intent.pitch);

            auto rot_raw = normalize(qYaw * tf.rotation * qPitch);

            auto f = normalize(forward(rot_raw));
            if(dot(forward(tf.rotation), f) > 0){
                auto r = normalize(cross(unitY(), f));
                auto u = cross(f, r);
                tf.rotation = quat(r, u, f);
            }
        }
    }
}

EditorControlSystem::EditorControlSystem(
    EntityRegistry& registry, IntentService& service)
:registry(registry), service(service){}

void EditorControlSystem::update(DeltaTime deltaTime){
    auto dt = deltaTime.count() / 1'000'000.0f;

    auto moveIntents = service.drainMoveIntents();
    auto lookIntents = service.drainLookIntents();

    for(auto [id, bit, tf, _2]: registry.query<Transform, Editor>()){
        for (const auto& intent : moveIntents) {
            Vec3 local = intent.move * dt;
            Vec3 world = rotate(local, tf.rotation);
            tf.position += world;
        }
        for(const auto& intent: lookIntents){
            auto qYaw   = axisAngle(unitY(), intent.yaw);
            auto qPitch = axisAngle(right(tf.rotation), intent.pitch);

            auto rot_raw = normalize(qPitch * qYaw * tf.rotation);

            auto f = normalize(forward(rot_raw));
            if(dot(normalize(ground_forward(tf.rotation)), f) > 0){
                auto r = normalize(cross(unitY(), f));
                auto u = cross(f, r);
                tf.rotation = quat(r, u, f);
            }
        }
    }
}

