#include "game/lifespan_system.hpp"
#include "task.hpp"
#include "app_state.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

LifespanSystem::LifespanSystem(AppState& app)
:app(app){}

size_t LifespanSystem::yield_count() const noexcept{
    size_t numTask = 0;

    for(const auto& [bit, gate]: app.archetypeMap){
        if(subset(bit_of<LifeSpanComponent>(), bit)){
            auto& vec=gate.raw();
            for(auto it=vec.cbegin(); it!=vec.cend(); ++it){
                auto lc = it.wrapped().at<LifeSpanComponent>(
                    offset_of<LifeSpanComponent>(bit));
                if(!lc.isAlive)
                    numTask += 1;
            }
        }
    }

    return numTask;
}

Generator<void> LifespanSystem::updateTask(DeltaTime) noexcept{

    for(const auto& [bit, gate]: app.archetypeMap){
        if(subset(bit_of<LifeSpanComponent>(), bit)){
            auto& vec=gate.raw();
            for(auto it=vec.cbegin(); it!=vec.cend(); ++it){
                auto lc = it.wrapped().at<LifeSpanComponent>(
                    offset_of<LifeSpanComponent>(bit));
                if(!lc.isAlive){
                    // destroy actor from Current epoch archetype map
                    app.destroyActor(lc.actor);
                    co_yield 0;
                }
            }
        }
    }
    co_return;
}

Generator<void> LifespanSystem::update(DeltaTime) noexcept{
    for(const auto& [bit, gate]: app.archetypeMap){
        if(subset(bit_of<LifeSpanComponent>(), bit)){
            auto& vec=gate.raw();
            for(auto it=vec.cbegin(); it!=vec.cend(); ++it){
                auto lc = it.wrapped().at<LifeSpanComponent>(
                    offset_of<LifeSpanComponent>(bit));
                if(!lc.isAlive){
                    // destroy actor from Previous epoch archetype map
                    app.destroyActor(lc.actor);
                    co_yield 0;
                }
            }
        }
    }

    co_return;
}
