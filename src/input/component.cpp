#include "input/component.hpp"

using namespace ModernBoy::Input;

Component::Component(EntityID actor,
    const Tasks& tasks, TransformHandle transformHandle)
:actor(actor), map(), transformHandle(transformHandle){
    for(const auto& task: tasks){
        auto [b_it, _] = map.insert({task.button, ButtonMap{}});
        b_it->second.insert({task.condition, task.behaviour});
    } 
}

Tasks Component::getTasks() const{
    Tasks tasks;

    for(const auto& bmap: map){
        auto button = bmap.first;
        for(const auto& c: bmap.second){
            Task newTask{button, c.first, c.second, transformHandle};
            tasks.emplace_back(std::move(newTask));
        }
    }

    return tasks;
}