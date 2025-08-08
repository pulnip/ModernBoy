#include <algorithm>
#include <string>
#include <unordered_map>
#include "engine/input/state.hpp"
#include "core/string/helper.hpp"

using namespace ModernBoy::Input;

bool ModernBoy::Input::isValid(KeyCode keyCode){
    return keyCode != KeyCode::KEY_UNKNOWN;
}
bool ModernBoy::Input::isValid(KeyState state){
    return state != KeyState::STATE_INVALID;
}

static std::unordered_map<std::string, KeyState>
text2state = {
    {"None", KeyState::None},
    {"HELD", KeyState::Held},
    {"RELEASED", KeyState::Released},
    {"PRESSED", KeyState::Pressed},
};
KeyState ModernBoy::Input::toKeyState(
    const std::string& text
){
    auto upper = toUpper(text);
    auto it = text2state.find(upper);
    if (it == text2state.end()){
        return KeyState::STATE_INVALID;
    }
    return it->second;
}
std::string ModernBoy::Input::toText(
    KeyState state
){
    switch(state){
    case KeyState::None:
        return "None";
    case KeyState::Held:
        return "Held";
    case KeyState::Released:
        return "Released";
    case KeyState::Pressed:
        return "Pressed";
    case STATE_INVALID: [[fallthrough]];
    default:
        break;
    }
    return "UNKNOWN";
}

SDL_Scancode ModernBoy::Input::convert(KeyCode code){
    switch(code){
    case KEY_0:    return SDL_SCANCODE_0;
    case KEY_1:    return SDL_SCANCODE_1;
    case KEY_2:    return SDL_SCANCODE_2;
    case KEY_3:    return SDL_SCANCODE_3;
    case KEY_4:    return SDL_SCANCODE_4;
    case KEY_5:    return SDL_SCANCODE_5;
    case KEY_6:    return SDL_SCANCODE_6;
    case KEY_7:    return SDL_SCANCODE_7;
    case KEY_8:    return SDL_SCANCODE_8;
    case KEY_9:    return SDL_SCANCODE_9;
    case KEY_TAB:  return SDL_SCANCODE_TAB;
    case KEY_Q:    return SDL_SCANCODE_Q;
    case KEY_W:    return SDL_SCANCODE_W;
    case KEY_E:    return SDL_SCANCODE_E;
    case KEY_R:    return SDL_SCANCODE_R;
    case KEY_T:    return SDL_SCANCODE_T;
    case KEY_Y:    return SDL_SCANCODE_Y;
    case KEY_U:    return SDL_SCANCODE_U;
    case KEY_I:    return SDL_SCANCODE_I;
    case KEY_O:    return SDL_SCANCODE_O;
    case KEY_P:    return SDL_SCANCODE_P;
    case KEY_A:    return SDL_SCANCODE_A;
    case KEY_S:    return SDL_SCANCODE_S;
    case KEY_D:    return SDL_SCANCODE_D;
    case KEY_F:    return SDL_SCANCODE_F;
    case KEY_G:    return SDL_SCANCODE_G;
    case KEY_H:    return SDL_SCANCODE_H;
    case KEY_J:    return SDL_SCANCODE_J;
    case KEY_K:    return SDL_SCANCODE_K;
    case KEY_L:    return SDL_SCANCODE_L;
    case KEY_Z:    return SDL_SCANCODE_Z;
    case KEY_X:    return SDL_SCANCODE_X;
    case KEY_C:    return SDL_SCANCODE_C;
    case KEY_V:    return SDL_SCANCODE_V;
    case KEY_B:    return SDL_SCANCODE_B;
    case KEY_N:    return SDL_SCANCODE_N;
    case KEY_M:    return SDL_SCANCODE_M;
    case KEY_SHIFT:return SDL_SCANCODE_LSHIFT;
    case KEY_CTRL: return SDL_SCANCODE_LCTRL;
    case KEY_ALT:  return SDL_SCANCODE_LALT;
    case KEY_SPACE:return SDL_SCANCODE_SPACE;
    case KEY_ENTER:return SDL_SCANCODE_RETURN;
    case KEY_ESC:  return SDL_SCANCODE_ESCAPE;
    case KEY_F1:   return SDL_SCANCODE_F1;
    case KEY_F2:   return SDL_SCANCODE_F2;
    case KEY_F3:   return SDL_SCANCODE_F3;
    case KEY_F4:   return SDL_SCANCODE_F4;
    case KEY_F5:   return SDL_SCANCODE_F5;
    case KEY_F6:   return SDL_SCANCODE_F6;
    case KEY_F7:   return SDL_SCANCODE_F7;
    case KEY_F8:   return SDL_SCANCODE_F8;
    case KEY_F9:   return SDL_SCANCODE_F9;
    case KEY_F10:  return SDL_SCANCODE_F10;
    case KEY_F11:  return SDL_SCANCODE_F11;
    case KEY_F12:  return SDL_SCANCODE_F12;
    case KEY_UP:   return SDL_SCANCODE_UP;
    case KEY_LEFT: return SDL_SCANCODE_LEFT;
    case KEY_DOWN: return SDL_SCANCODE_DOWN;
    case KEY_RIGHT:return SDL_SCANCODE_RIGHT;
    default:       return SDL_SCANCODE_UNKNOWN;
    }
}
KeyCode ModernBoy::Input::convert(SDL_Scancode code){
    switch(code){
    case SDL_SCANCODE_0:        return KEY_0;
    case SDL_SCANCODE_1:        return KEY_1;
    case SDL_SCANCODE_2:        return KEY_2;
    case SDL_SCANCODE_3:        return KEY_3;
    case SDL_SCANCODE_4:        return KEY_4;
    case SDL_SCANCODE_5:        return KEY_5;
    case SDL_SCANCODE_6:        return KEY_6;
    case SDL_SCANCODE_7:        return KEY_7;
    case SDL_SCANCODE_8:        return KEY_8;
    case SDL_SCANCODE_9:        return KEY_9;
    case SDL_SCANCODE_TAB:      return KEY_TAB;
    case SDL_SCANCODE_Q:        return KEY_Q;
    case SDL_SCANCODE_W:        return KEY_W;
    case SDL_SCANCODE_E:        return KEY_E;
    case SDL_SCANCODE_R:        return KEY_R;
    case SDL_SCANCODE_T:        return KEY_T;
    case SDL_SCANCODE_Y:        return KEY_Y;
    case SDL_SCANCODE_U:        return KEY_U;
    case SDL_SCANCODE_I:        return KEY_I;
    case SDL_SCANCODE_O:        return KEY_O;
    case SDL_SCANCODE_P:        return KEY_P;
    case SDL_SCANCODE_A:        return KEY_A;
    case SDL_SCANCODE_S:        return KEY_S;
    case SDL_SCANCODE_D:        return KEY_D;
    case SDL_SCANCODE_F:        return KEY_F;
    case SDL_SCANCODE_G:        return KEY_G;
    case SDL_SCANCODE_H:        return KEY_H;
    case SDL_SCANCODE_J:        return KEY_J;
    case SDL_SCANCODE_K:        return KEY_K;
    case SDL_SCANCODE_L:        return KEY_L;
    case SDL_SCANCODE_Z:        return KEY_Z;
    case SDL_SCANCODE_X:        return KEY_X;
    case SDL_SCANCODE_C:        return KEY_C;
    case SDL_SCANCODE_V:        return KEY_V;
    case SDL_SCANCODE_B:        return KEY_B;
    case SDL_SCANCODE_N:        return KEY_N;
    case SDL_SCANCODE_M:        return KEY_M;
    case SDL_SCANCODE_LSHIFT:   [[fallthrough]];
    case SDL_SCANCODE_RSHIFT:   return KEY_SHIFT;
    case SDL_SCANCODE_LCTRL:    [[fallthrough]];
    case SDL_SCANCODE_RCTRL:    return KEY_CTRL;
    case SDL_SCANCODE_LALT:     [[fallthrough]];
    case SDL_SCANCODE_RALT:     return KEY_ALT;
    case SDL_SCANCODE_ESCAPE:   return KEY_ESC;
    case SDL_SCANCODE_F1:       return KEY_F1;
    case SDL_SCANCODE_F2:       return KEY_F2;
    case SDL_SCANCODE_F3:       return KEY_F3;
    case SDL_SCANCODE_F4:       return KEY_F4;
    case SDL_SCANCODE_F5:       return KEY_F5;
    case SDL_SCANCODE_F6:       return KEY_F6;
    case SDL_SCANCODE_F7:       return KEY_F7;
    case SDL_SCANCODE_F8:       return KEY_F8;
    case SDL_SCANCODE_F9:       return KEY_F9;
    case SDL_SCANCODE_F10:      return KEY_F10;
    case SDL_SCANCODE_F11:      return KEY_F11;
    case SDL_SCANCODE_F12:      return KEY_F12;
    case SDL_SCANCODE_UP:       return KEY_UP;
    case SDL_SCANCODE_LEFT:     return KEY_LEFT;
    case SDL_SCANCODE_DOWN:     return KEY_DOWN;
    case SDL_SCANCODE_RIGHT:    return KEY_RIGHT;
    case SDL_SCANCODE_SPACE:    return KEY_SPACE;
    case SDL_SCANCODE_RETURN:   return KEY_ENTER;
    default:                    return KEY_UNKNOWN;
    }
}
static std::unordered_map<std::string, KeyCode>
text2keyCode = {
    {"0"        , KeyCode::KEY_0},
    {"1"        , KeyCode::KEY_1},
    {"2"        , KeyCode::KEY_2},
    {"3"        , KeyCode::KEY_3},
    {"4"        , KeyCode::KEY_4},
    {"5"        , KeyCode::KEY_5},
    {"6"        , KeyCode::KEY_6},
    {"7"        , KeyCode::KEY_7},
    {"8"        , KeyCode::KEY_8},
    {"9"        , KeyCode::KEY_9},
    {"TAB"      , KeyCode::KEY_TAB},
    {"Q"        , KeyCode::KEY_Q},
    {"W"        , KeyCode::KEY_W},
    {"E"        , KeyCode::KEY_E},
    {"R"        , KeyCode::KEY_R},
    {"T"        , KeyCode::KEY_T},
    {"Y"        , KeyCode::KEY_Y},
    {"U"        , KeyCode::KEY_U},
    {"I"        , KeyCode::KEY_I},
    {"O"        , KeyCode::KEY_O},
    {"P"        , KeyCode::KEY_P},
    {"A"        , KeyCode::KEY_A},
    {"S"        , KeyCode::KEY_S},
    {"D"        , KeyCode::KEY_D},
    {"F"        , KeyCode::KEY_F},
    {"G"        , KeyCode::KEY_G},
    {"H"        , KeyCode::KEY_H},
    {"J"        , KeyCode::KEY_J},
    {"K"        , KeyCode::KEY_K},
    {"L"        , KeyCode::KEY_L},
    {"Z"        , KeyCode::KEY_Z},
    {"X"        , KeyCode::KEY_X},
    {"C"        , KeyCode::KEY_C},
    {"V"        , KeyCode::KEY_V},
    {"B"        , KeyCode::KEY_B},
    {"N"        , KeyCode::KEY_N},
    {"M"        , KeyCode::KEY_M},
    {"SHIFT"    , KeyCode::KEY_SHIFT},
    {"CTRL"     , KeyCode::KEY_CTRL},
    {"ALT"      , KeyCode::KEY_ALT},
    {"ESC"      , KeyCode::KEY_ESC},
    {"F1"       , KeyCode::KEY_F1},
    {"F2"       , KeyCode::KEY_F2},
    {"F3"       , KeyCode::KEY_F3},
    {"F4"       , KeyCode::KEY_F4},
    {"F5"       , KeyCode::KEY_F5},
    {"F6"       , KeyCode::KEY_F6},
    {"F7"       , KeyCode::KEY_F7},
    {"F8"       , KeyCode::KEY_F8},
    {"F9"       , KeyCode::KEY_F9},
    {"F10"      , KeyCode::KEY_F10},
    {"F11"      , KeyCode::KEY_F11},
    {"F12"      , KeyCode::KEY_F12},
    {"UP"       , KeyCode::KEY_UP},
    {"LEFT"     , KeyCode::KEY_LEFT},
    {"DOWN"     , KeyCode::KEY_DOWN},
    {"RIGHT"    , KeyCode::KEY_RIGHT},
    {"SPACE"    , KeyCode::KEY_SPACE},
    {"ENTER"    , KeyCode::KEY_ENTER}
};
KeyCode ModernBoy::Input::toKey(
    const std::string& text
){
    auto upper = toUpper(text);
    auto it = text2keyCode.find(upper);
    if (it == text2keyCode.end()){
        return KeyCode::KEY_UNKNOWN;
    }
    return it->second;
}
std::string ModernBoy::Input::toText(
    KeyCode keyCode
){
    switch (keyCode) {
    case KeyCode::KEY_0:     return "0";
    case KeyCode::KEY_1:     return "1";
    case KeyCode::KEY_2:     return "2";
    case KeyCode::KEY_3:     return "3";
    case KeyCode::KEY_4:     return "4";
    case KeyCode::KEY_5:     return "5";
    case KeyCode::KEY_6:     return "6";
    case KeyCode::KEY_7:     return "7";
    case KeyCode::KEY_8:     return "8";
    case KeyCode::KEY_9:     return "9";
    case KeyCode::KEY_TAB:   return "TAB";
    case KeyCode::KEY_Q:     return "Q";
    case KeyCode::KEY_W:     return "W";
    case KeyCode::KEY_E:     return "E";
    case KeyCode::KEY_R:     return "R";
    case KeyCode::KEY_T:     return "T";
    case KeyCode::KEY_Y:     return "Y";
    case KeyCode::KEY_U:     return "U";
    case KeyCode::KEY_I:     return "I";
    case KeyCode::KEY_O:     return "O";
    case KeyCode::KEY_P:     return "P";
    case KeyCode::KEY_A:     return "A";
    case KeyCode::KEY_S:     return "S";
    case KeyCode::KEY_D:     return "D";
    case KeyCode::KEY_F:     return "F";
    case KeyCode::KEY_G:     return "G";
    case KeyCode::KEY_H:     return "H";
    case KeyCode::KEY_J:     return "J";
    case KeyCode::KEY_K:     return "K";
    case KeyCode::KEY_L:     return "L";
    case KeyCode::KEY_Z:     return "Z";
    case KeyCode::KEY_X:     return "X";
    case KeyCode::KEY_C:     return "C";
    case KeyCode::KEY_V:     return "V";
    case KeyCode::KEY_B:     return "B";
    case KeyCode::KEY_N:     return "N";
    case KeyCode::KEY_M:     return "M";
    case KeyCode::KEY_SHIFT: return "SHIFT";
    case KeyCode::KEY_CTRL:  return "CTRL";
    case KeyCode::KEY_ALT:   return "ALT";
    case KeyCode::KEY_ESC:   return "ESC";
    case KeyCode::KEY_F1:    return "F1";
    case KeyCode::KEY_F2:    return "F2";
    case KeyCode::KEY_F3:    return "F3";
    case KeyCode::KEY_F4:    return "F4";
    case KeyCode::KEY_F5:    return "F5";
    case KeyCode::KEY_F6:    return "F6";
    case KeyCode::KEY_F7:    return "F7";
    case KeyCode::KEY_F8:    return "F8";
    case KeyCode::KEY_F9:    return "F9";
    case KeyCode::KEY_F10:   return "F10";
    case KeyCode::KEY_F11:   return "F11";
    case KeyCode::KEY_F12:   return "F12";
    case KeyCode::KEY_UP:    return "UP";
    case KeyCode::KEY_LEFT:  return "LEFT";
    case KeyCode::KEY_DOWN:  return "DOWN";
    case KeyCode::KEY_RIGHT: return "RIGHT";
    case KeyCode::KEY_SPACE: return "SPACE";
    case KeyCode::KEY_ENTER: return "ENTER";
    case KeyCode::KEY_UNKNOWN: [[fallthrough]];
    default:
        break;
    }
    return "UNKNOWN";
}

KeyState ModernBoy::Input::transit(
    KeyState state, uint8_t current
){
    uint8_t changed = ((state & ACTIVE_FLAG) ^ current) << 1;
    return static_cast<KeyState>(changed | current);
}
