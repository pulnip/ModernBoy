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
        if(input.query(KEY_A, Held))
            tc.position -= 10 * dt_ * ground_right(tc.rotation);
        if(input.query(KEY_D, Held))
            tc.position += 10 * dt_ * ground_right(tc.rotation);
        if(input.query(KEY_W, Held))
            tc.position += 10 * dt_ * ground_forward(tc.rotation);
        if(input.query(KEY_S, Held))
            tc.position -= 10 * dt_ * ground_forward(tc.rotation);

        if(input.query(KEY_Q, Held))
            tc.rotation = rotateY(-3.14/2 * dt_) * tc.rotation;
        if(input.query(KEY_E, Held))
            tc.rotation = rotateY( 3.14/2 * dt_) * tc.rotation;
        // if(input.query(KEY_SPACE, Pressed))
        //     jump;
    }
}