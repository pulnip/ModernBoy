#include "camera_ray_system.hpp"
#include "engine/service/input_service.hpp"
#include "entity_registry.hpp"
#include "ray_service.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;
using namespace ModernBoy::Service;

CameraRaySystem::CameraRaySystem(EntityRegistry& registry,
    InputService& inputSrv, RayService& raySrv)
:registry(registry), inputService(inputSrv)
,rayService(raySrv){}

void CameraRaySystem::update(DeltaTime deltaTime){
    auto dt = deltaTime.count() / 1'000'000.0f;

    for(auto [id, bit, tf, cam]: registry.query<Transform, Camera>()){
        auto mouseState = inputService.snapshot().mouse;
        auto fov = cam.fov;

        Ray ray{
            .point = tf.position,
            .dir = normalize(
                mouseState.x * right(tf.rotation) +
                mouseState.y * up(tf.rotation) +
                1/std::tanf(fov/2) * forward(tf.rotation)
            )
        };
        rayService.write(ray);
    }

}
