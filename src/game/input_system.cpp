#include <cassert>
#include "game/context.hpp"
#include "game/input_system.hpp"
#include "game/component.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

InputSystem::InputSystem(Context& world)
:world(world){}

size_t InputSystem::yield_count() const noexcept{
    numTask = 0;

    for(const auto& [bit, vec]: world.query<
        TransformComponent, MeshComponent>()
    ){
        if(subset(bit_of<InputTask>(), bit))
            numTask += vec.size();
    }

    return numTask;
}
Generator<void> InputSystem::updateTask(DeltaTime){
    inputTasks.reserve(numTask);
    inputTasks.clear();

    for(const auto& [bit, vec]:
        world.query<InputComponent>()
    ){
        if(!subset(bit_of<InputTask>(), bit))
            continue;
        vec.for_each([this](const InputComponent& ic){
            if(ic.isActive)
                inputTasks.emplace_back(
                    InputTask{.actor=ic.actor}
                );
        });
        co_yield 0;
    }

    co_return;

}
Generator<void> InputSystem::update(DeltaTime){
    for(const auto& task: inputTasks){
        world.on<Event::PlayerMove>();
        co_yield 0;
    }

    co_return;
}
