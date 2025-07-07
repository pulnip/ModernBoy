#include <cassert>
#include "log.hpp"
#include "game/context.hpp"
#include "game/draw_system.hpp"
#include "game/component.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

DrawSystem::DrawSystem(Context& world)
:world(world){}

size_t DrawSystem::yield_count() const noexcept{

    size_t numViewTask = 0;
    size_t numDrawTask = 0;

    for(const auto& [bit, vec]: world.query<
        TransformComponent, CameraComponent>()
    ){
        if(subset(bit_of<ViewTask>(), bit))
            numViewTask += 1;
    }
    for(const auto& [bit, vec]: world.query<
        TransformComponent, MeshComponent>()
    ){    
        if(subset(bit_of<DrawTask>(), bit))
            numDrawTask += 1;
    }

    GameTrace("Num View Task: {}, Num Draw Task: {}",
        numViewTask, numDrawTask);

    return 1 * numDrawTask;
}
Generator<void> DrawSystem::update(DeltaTime){
    viewTasks.clear();
    drawTasks.clear();

    for(const auto& [bit, vec]: world.query<
        TransformComponent, CameraComponent>()
    ){
        GameTrace("query ArchetypeMap, bit: {}, vec size: {}",
            bit, vec.size());
        if(!subset(bit_of<ViewTask>(), bit))
            GameCritical("ArchetypeMap is something wrong!");

        viewTasks.reserve(vec.size());

        vec.for_each([this](
            const TransformComponent& tc,
            const CameraComponent& cc
        ){
            assert(tc.actor == cc.actor);
            if(cc.isActive)
                viewTasks.emplace_back(ViewTask{
                    tc.value, cc.value});
        });

        co_yield 0;

        for(const auto& [bit, vec]: world.query<
            TransformComponent, MeshComponent>()
        ){
            GameTrace("query ArchetypeMap, bit: {}, vec size: {}",
                bit, vec.size());
            if(!subset(bit_of<DrawTask>(), bit))
                GameCritical("ArchetypeMap is something wrong!");

            drawTasks.reserve(vec.size());

            vec.for_each([this](
                const TransformComponent& tc,
                const MeshComponent& mc
            ){
                assert(tc.actor == mc.actor);
                if(mc.isActive)
                    drawTasks.emplace_back(DrawTask{
                        tc.value, mc.handle,
                        mc.textureHandle,
                        mc.shaderHandle});
            });

            co_yield 0;
        }
    }

    co_return;
}
