#include "input/system.hpp"
#include "app_state.hpp"
#include "task.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Input;

System::System(AppState& app)
:app(app){}

static InputTask parseComponent(const void* chunk);

void System::update(DeltaTime dt){
    std::vector<InputTask> tasks;

    for(const auto& [bit, vec]: app.archetypeMap){
        tasks.reserve(tasks.size()+vec.size());
        if(!subset(bit_of<InputTask>(), bit))
            continue;
        vec.for_each([&tasks](const void* chunk){
            tasks.emplace_back(parseComponent(chunk));
        });
    }
}

static InputTask parseComponent(const void* chunk){
    InputTask task;
    TransformComponent tc;
    InputComponent ic;
    getChunk(&tc, nullptr, nullptr, &ic, chunk);
    assert(tc.actor == ic.actor);

    // app.input...
    return task;
}
