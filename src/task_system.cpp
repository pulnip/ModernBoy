#include <cassert>
#include "task_system.hpp"
#include "app_state.hpp"

using namespace ModernBoy;

template<typename T> T& get(void* ptr){
    return *static_cast<T*>(ptr); }
template<typename T> const T& get(const void* ptr){
    return *static_cast<const T*>(ptr); }

template<typename T> void* move_ptr(void* ptr){
    return static_cast<uint8_t*>(ptr)+sizeof(T); }
template<typename T> const void* move_ptr(const void* ptr){
    return static_cast<const uint8_t*>(ptr)+sizeof(T); }

template<> std::vector<RenderTask>
ComponentSystem::getTask<RenderTask>(){
    std::vector<RenderTask> tasks;

    for(const auto& [bit, vec]: app.archetypeMap){
        if(!subset(bit_of<RenderTask>(), bit))
            continue;
        for(const auto& chunk: vec){
            const void* ptr = chunk;
            const auto& tc = ::get<TransformComponent>(ptr);
            ptr = move_ptr<TransformComponent>(ptr);
            const auto& mc = ::get<MeshComponent>(ptr);
            ptr = move_ptr<MeshComponent>(ptr);

            assert(tc.actor == mc.actor);
            const auto& meshVec = app.meshManager.get(mc.accessHandle);
            tasks.reserve(tasks.size() + meshVec.size());
            for(const auto& meshHandle: meshVec){
                tasks.emplace_back(RenderTask{
                    tc.value, meshHandle
                });
            }
        }
    }

    return tasks;
}

template<> std::vector<ViewTask>
ComponentSystem::getTask<ViewTask>(){
    std::vector<ViewTask> tasks;
}

template<> std::vector<InputTask>
ComponentSystem::getTask<InputTask>(){
    std::vector<InputTask> tasks;
}