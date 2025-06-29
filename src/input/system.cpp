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
        // ToDo. actor 0 fixed
        app.scriptInvoker.invoke(module, task.function, 0);
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
            TransformComponent tc;
            InputComponent ic;
            getChunk(&tc, nullptr, nullptr, &ic, chunk, bit);

            // std::println("actor: {}, num Action: {}", ic.actor, ic.numAction);

            for(size_t i=0; i<ic.numAction; ++i){
                if(inputState.keyState[ic.triggers[i].button]
                    == ic.triggers[i].onState
                ){
                    tasks.emplace_back(InputTask{
                        .function = ic.actions[i].function,
                        .handle = ic.actions[i].moduleHandle,
                        .transform = tc.value
                    });
                }

            }
        });
    }
    return tasks;
}

