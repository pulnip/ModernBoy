#include "input/chord.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Input;

bool Chord::query(KeyCode keyCode, KeyState keyState
){
    return state.keyboard[keyCode] == keyState;
}