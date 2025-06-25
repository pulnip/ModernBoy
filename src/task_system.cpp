#include <cassert>
#include "common/alias.hpp"
#include "util/bit.hpp"
#include "task_system.hpp"
#include "app_state.hpp"

using namespace ModernBoy;

template<> std::vector<RenderTask>
TaskSystem::getTask<RenderTask>(){
    size_t size = 0;
    // calculate size only
    for(const auto& [bit, vec]: app.archetypeMap){
        if(!subset(bit_of<RenderTask>(), bit))
            continue;
        vec.for_each([&size, this](Index, const void* chunk){
            TransformComponent tc;
            MeshComponent mc;
            getChunk(&tc, nullptr, &mc, nullptr, chunk);
            assert(tc.actor == mc.actor);

            size += app.meshManager.get(mc.accessHandle).size();
        });
    }

    std::vector<RenderTask> tasks(size);

    for(const auto& [bit, vec]: app.archetypeMap){
        if(!subset(bit_of<RenderTask>(), bit))
            continue;
        vec.for_each([&tasks, this](Index, const void* chunk){
            TransformComponent tc;
            MeshComponent mc;
            getChunk(&tc, nullptr, &mc, nullptr, chunk);
    
            assert(tc.actor == mc.actor);
            const auto& meshVec = app.meshManager.get(mc.accessHandle);
            for(const auto& meshHandle: meshVec){
                tasks.emplace_back(RenderTask{
                    tc.value, meshHandle
                });
            }
        });
    }

    return tasks;
}

template<> std::vector<ViewTask>
TaskSystem::getTask<ViewTask>(){
    size_t size = 0;
    // calculate size only
    for(const auto& [bit, vec]: app.archetypeMap){
        if(!subset(bit_of<ViewTask>(), bit))
            continue;
        size += vec.size();
    }

    std::vector<ViewTask> tasks(size);

    for(const auto& [bit, vec]: app.archetypeMap){
        if(!subset(bit_of<ViewTask>(), bit))
            continue;
                vec.for_each([&tasks](Index, const void* chunk){
            TransformComponent tc;
            CameraComponent cc;
            getChunk(&tc, &cc, nullptr, nullptr, chunk);
    
            assert(tc.actor == cc.actor);
            tasks.emplace_back(ViewTask{
                tc.value, cc.value
            });
        });
    }

    return tasks;
}

// template<> std::vector<InputTask>
// TaskSystem::getTask<InputTask>(){
//     std::vector<InputTask> tasks;
// }