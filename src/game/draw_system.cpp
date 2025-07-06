#include <cassert>
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
        TransformComponent, MeshComponent>()
    ){
        if(subset(bit_of<ViewTask>(), bit))
            numViewTask += 1;
        if(subset(bit_of<DrawTask>(), bit))
            numDrawTask += 1;
    }

    return numViewTask * numDrawTask;
}
Generator<void> DrawSystem::updateTask(DeltaTime){
    viewTasks.clear();
    drawTasks.clear();

    for(const auto& [bit, vec]: world.query<
        TransformComponent, MeshComponent>()
    ){
        if(subset(bit_of<ViewTask>(), bit)){
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
        }
        if(subset(bit_of<DrawTask>(), bit)){
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
Generator<void> DrawSystem::update(DeltaTime){
    

    for(const auto& task: viewTasks){
        for(const auto& task: drawTasks){


            co_yield 0;
        }
        co_yield 0;
    }
    co_return;
}
