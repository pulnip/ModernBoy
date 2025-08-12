#include <cassert>
#include "engine/log.hpp"
#include "entity_registry.hpp"
#include "draw_system.hpp"
#include "component.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

DrawSystem::DrawSystem(EntityRegistry& registry)
:registry(registry){}

void DrawSystem::update(){
    viewTasks.clear();
    drawTasks.clear();

    for(const auto [id, bit, tc, cc]: registry.query<
        Transform, Camera>()
    ){
        assert(tc.entity == cc.entity);
        if(cc.isActive)
            viewTasks.emplace_back(ViewTask{
                tc.position, tc.rotation, tc.scale,
                cc.type, cc.fov, cc.nearPlane, cc.farPlane,
                cc.projection});
    }
    for(const auto [id, bit, tc, mc]: registry.query<Transform, Model>()
    ){
        assert(tc.entity == mc.entity);
        if(mc.isActive)
            drawTasks.emplace_back(DrawTask{
                tc.position, tc.rotation, tc.scale,
                mc.alpha,
                mc.handle,
                mc.textureHandle,
                mc.shaderHandle,
                id});
    }
}

// size_t DrawSystem::yield_count() noexcept{

//     size_t numViewTask = 0;
//     size_t numDrawTask = 0;

//     for(const auto _: registry.query<
//         Transform, Camera>()
//     ){
//         numViewTask += 1;
//     }
//     for(const auto _: registry.query<
//         Transform, Mesh>()
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
//         Transform, Camera>()
//     ){
//         assert(tc.entity == cc.entity);
//         if(cc.isActive)
//             viewTasks.emplace_back(ViewTask{
//                 tc.position, tc.rotation, tc.scale,
//                 cc.value});

//         co_yield 0;
//     }
//     for(const auto [id, bit, tc, mc]: registry.query<
//         Transform, Mesh>()
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
