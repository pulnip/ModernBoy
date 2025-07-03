#include "input/system.hpp"
#include "app_state.hpp"
#include "task.hpp"

#include <print>

using namespace ModernBoy;
using namespace ModernBoy::Input;

System::System(AppState& app)
:app(app){
    state.keyState.fill(ButtonState::None);
}

using InputTasks = std::vector<InputTask>;

size_t System::yield_count() const noexcept{
    size_t numTask = 0;

    for(const auto& [bit, gate]: app.archetypeMap){
        if(!subset(bit_of<InputTask>(), bit))
            continue;
        numTask += 1;
    }

    return numTask;
}

Generator<void> System::updateTask(DeltaTime){
    device.fetch(state);
    inputTasks.clear();

    for(const auto& [bit, vec]: app.archetypeMap){
        if(!subset(bit_of<InputTask>(), bit))
            continue;
        vec.for_each(
            [this](const InputComponent& ic){
                inputTasks.reserve(inputTasks.size()+ic.numAction);

                for(size_t i=0; i<ic.numAction; ++i){
                    inputTasks.emplace_back(InputTask{
                        .actor = ic.actor,
                        .function = ic.actions[i].function,
                        .handle = ic.actions[i].moduleHandle,
                        .trigger = ic.triggers[i]
                    });
                }
            }
        );
        co_yield 0;
    }
    co_return;
}

Generator<void> System::update(DeltaTime){
    for(const auto& task: inputTasks){
        auto button = task.trigger.button;
        auto onState = task.trigger.onState;

        if(state.keyState[button] == onState){
            const auto& module = app.get<Script::Module>(
                task.handle);
            app.scriptInvoker.invokeInput(
                module, task.function,
                task.actor, {button, onState});
        }
        co_yield 0;
    }

    co_return;
}
