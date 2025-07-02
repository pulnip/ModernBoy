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

static InputTasks fetchTask(const ArchetypeMap& map,
    const Input::State& state);

void System::update([[maybe_unused]] DeltaTime dt){
    device.fetch(state);
    auto tasks = fetchTask(app.archetypeMap, state);

    for(const auto& task: tasks){
        const auto& module = app.get<Script::Module>(task.handle);
        app.scriptInvoker.invokeInput(module, task.function,
            task.actor, task.trigger);
    }
}

static InputTasks fetchTask(const ArchetypeMap& map,
    const Input::State& inputState
){
        std::vector<InputTask> tasks;

    for(const auto& [bit, vec]: map){
        tasks.reserve(tasks.size()+vec.size());
        if(!subset(bit_of<InputTask>(), bit))
            continue;
        vec.for_each([&tasks, &inputState, bit](const void* chunk){
            InputComponent ic;
            getChunk(nullptr, nullptr, nullptr, &ic,
                chunk, bit);

            // std::println("actor: {}, num Action: {}", ic.actor, ic.numAction);

            for(size_t i=0; i<ic.numAction; ++i){
                auto button = ic.triggers[i].button;
                auto onState = ic.triggers[i].onState;
                if(inputState.keyState[button] == onState){
                    tasks.emplace_back(InputTask{
                        .actor = ic.actor,
                        .function = ic.actions[i].function,
                        .handle = ic.actions[i].moduleHandle,
                        .trigger = {button, onState}
                    });
                }

            }
        });
    }
    return tasks;
}

size_t System::yield_count() const noexcept{
    size_t numTask = 0;

    for(const auto& [bit, gate]: app.archetypeMap){
        const auto& vec = gate.raw();
        if(subset(bit_of<InputTask>(), bit))
            numTask += vec.size();
    }

    return numTask;
}

Generator<void> System::updateV2(DeltaTime dt){
    device.fetch(state);

    for(const auto& [bit, gate]: app.archetypeMap){
        if(!subset(bit_of<InputTask>(), bit))
            continue;
        auto& vec=gate.raw();
        for(auto it=vec.cbegin(); it!=vec.cend(); ++it){
            auto ic = it.wrapped().at<InputComponent>(
                offset_of<InputComponent>(bit)
            );

            for(size_t i=0; i<ic.numAction; ++i){
                auto button = ic.triggers[i].button;
                auto onState = ic.triggers[i].onState;

                if(state.keyState[button] == onState){
                    const auto& module = app.get<Script::Module>(
                        ic.actions[i].moduleHandle);
                    app.scriptInvoker.invokeInput(
                        module, ic.actions[i].function,
                        ic.actor, {button, onState});
                }
            }

            co_yield 0;
        }
    }

    co_return;
}
