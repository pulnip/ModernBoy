#include <cassert>
#include "log.hpp"
#include "game/entity_registry.hpp"
#include "game/draw_system.hpp"
#include "game/component.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

DrawSystem::DrawSystem(EntityRegistry& registry)
:registry(registry){}

void DrawSystem::update(DeltaTime){
    viewTasks.clear();
    drawTasks.clear();

    for(const auto [id, bit, tc, cc]: registry.query<
        TransformComponent, CameraComponent>()
    ){
        assert(tc.entity == cc.entity);
        if(cc.isActive)
            viewTasks.emplace_back(ViewTask{
                tc.position, tc.rotation, tc.scale,
                cc.type, cc.fov, cc.nearPlane, cc.farPlane,
                cc.projection});
    }
    for(const auto [id, bit, tc, mc]: registry.query<
        TransformComponent, MeshComponent>()
    ){
        assert(tc.entity == mc.entity);
        if(mc.isActive)
            drawTasks.emplace_back(DrawTask{
                tc.position, tc.rotation, tc.scale,
                mc.alpha,
                mc.handle,
                mc.textureHandle,
                mc.shaderHandle});
    }
}

// size_t DrawSystem::yield_count() noexcept{

//     size_t numViewTask = 0;
//     size_t numDrawTask = 0;

//     for(const auto _: registry.query<
//         TransformComponent, CameraComponent>()
//     ){
//         numViewTask += 1;
//     }
//     for(const auto _: registry.query<
//         TransformComponent, MeshComponent>()
//     ){    
//         numDrawTask += 1;
//     }

//     GameTrace("Num View Task: {}, Num Draw Task: {}",
//         numViewTask, numDrawTask);

//     viewTasks.reserve(numViewTask);
//     drawTasks.reserve(numDrawTask);

//     return 1 * numDrawTask;
// }
// Generator<void> DrawSystem::update(DeltaTime){
//     viewTasks.clear();
//     drawTasks.clear();

//     for(const auto [id, bit, tc, cc]: registry.query<
//         TransformComponent, CameraComponent>()
//     ){
//         assert(tc.entity == cc.entity);
//         if(cc.isActive)
//             viewTasks.emplace_back(ViewTask{
//                 tc.position, tc.rotation, tc.scale,
//                 cc.value});

//         co_yield 0;
//     }
//     for(const auto [id, bit, tc, mc]: registry.query<
//         TransformComponent, MeshComponent>()
//     ){
//         assert(tc.entity == mc.entity);
//         if(mc.isActive)
//             drawTasks.emplace_back(DrawTask{
//                 tc.position, tc.rotation, tc.scale,
//                 mc.alpha,
//                 mc.handle,
//                 mc.textureHandle,
//                 mc.shaderHandle});

//         co_yield 0;
//     }

//     co_return;
// }
