#include "input/component.hpp"

using namespace ModernBoy::Input;

Component::Component(EntityID actor,
    const Behaviours& behaviours, TransformHandle transformHandle)
:actor(actor), map(), transformHandle(transformHandle){
    for(const auto& behaviour: behaviours){
        auto [b_it, _] = map.insert({behaviour.button, ButtonMap{}});
        b_it->second.insert({behaviour.condition, behaviour.behaviour});
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