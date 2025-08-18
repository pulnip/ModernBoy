#include <algorithm>
#include <string>
#include <unordered_map>
#include <SDL3/SDL_keycode.h>
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

[[maybe_unused]] static KeyCode convert(SDL_Scancode code){
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
