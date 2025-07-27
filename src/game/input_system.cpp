#include "input/chord.hpp"
#include "game/entity_registry.hpp"
#include "game/input_system.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;
using namespace ModernBoy::Input;

InputSystem::InputSystem(EntityRegistry& registry,
    ModernBoy::Input::Chord& input)
:registry(registry), input(input){}

void InputSystem::update(DeltaTime dt){
    auto dt_ = dt.count() / 1'000'000.0f;

    for(auto [id, bit, tc, _1]: registry.query<Transform, Player>()){
        if(input.query(KEY_LEFT, Held))
            tc.position -= 10 * dt_ * ground_right(tc.rotation);
        if(input.query(KEY_RIGHT, Held))
            tc.position += 10 * dt_ * ground_right(tc.rotation);
        if(input.query(KEY_UP, Held))
            tc.position -= 10 * dt_ * ground_forward(tc.rotation);
        if(input.query(KEY_DOWN, Held))
            tc.position += 10 * dt_ * ground_forward(tc.rotation);
        // if(input.query(KEY_SPACE, Pressed))
        //     jump;
    }
}