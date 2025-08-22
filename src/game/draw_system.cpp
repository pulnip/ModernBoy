#include <cassert>
#include "engine/log/log.hpp"
#include "engine/interface/view_service.hpp"
#include "engine/interface/draw_service.hpp"
#include "entity_registry.hpp"
#include "draw_system.hpp"
#include "component.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Interface;
using namespace ModernBoy::Game;

DrawSystem::DrawSystem(EntityRegistry& registry,
    const MeshTable& meshTable,
    const MaterialSetTable& materialSetTable,
    ViewService& viewSrv, DrawService& drawSrv)
:registry(registry), meshTable(meshTable),
materialSetTable(materialSetTable),
viewService(viewSrv), drawService(drawSrv){}

void DrawSystem::update(){
    for(const auto [id, bit, tc, cc]: registry.query<
        Transform, Camera>()
    ){
        assert(tc.entity == cc.entity);
        if(cc.isActive)
            viewService.write(CameraObject{
                tc.position, tc.rotation, tc.scale,
                cc.type, cc.fov, cc.nearPlane, cc.farPlane,
                cc.projection});
    }
    for(const auto [id, bit, tc, mc]: registry.query<Transform, Mesh>()){
        const auto& submeshes = meshTable.at(mc.mesh);
        const auto& materials = materialSetTable.at(mc.materialSet);

        for(Index i=0; i<submeshes.size(); ++i){
            drawService.write(MeshDrawCall{
                tc.position, tc.rotation, tc.scale,
                mc.alpha, submeshes[i],
                materials[i],
                mc.shaderHandle,
                id
            });
        }
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
