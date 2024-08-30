#include "InputState.hpp"

using namespace std;
using namespace ModernBoy;

bool KeyboardState::isPressed(uint8_t kbKey) const noexcept{
    return current[kbKey];
}
