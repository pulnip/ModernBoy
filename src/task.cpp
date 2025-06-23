#include "task.hpp"

using namespace ModernBoy::Input;

void ModernBoy::Input::addInput(InputMap& map,
    Button button, ButtonState state,
    const std::string& behaviour
){
    auto it = map.find(button);
    [[unlikely]] if(it == map.end()){
        map.emplace(std::make_pair(
            button, ButtonMap{
                std::make_pair(state, behaviour)
            }
        ));
    }
}