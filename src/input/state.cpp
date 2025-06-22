#include <algorithm>
#include <string>
#include <unordered_map>
#include "input/state.hpp"

using namespace ModernBoy::Input;

static std::string toUpper(const std::string& text);

static std::unordered_map<std::string, ButtonState>
text2state = {
    {"NONE", ButtonState::None},
    {"HELD", ButtonState::Held},
    {"RELEASED", ButtonState::Released},
    {"PRESSED", ButtonState::Pressed},
};
ButtonState ModernBoy::Input::toButtonState(
    const std::string& text
){
    auto upper = toUpper(text);
    auto it = text2state.find(upper);
    if (it == text2state.end()){
        return ButtonState::STATE_INVALID;
    }
    return it->second;
}

SDL_Scancode ModernBoy::Input::convert(Button code){
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
Button ModernBoy::Input::convert(SDL_Scancode code){
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
static std::unordered_map<std::string, Button>
text2button = {
    {"0"        , Button::KEY_0},
    {"1"        , Button::KEY_1},
    {"2"        , Button::KEY_2},
    {"3"        , Button::KEY_3},
    {"4"        , Button::KEY_4},
    {"5"        , Button::KEY_5},
    {"6"        , Button::KEY_6},
    {"7"        , Button::KEY_7},
    {"8"        , Button::KEY_8},
    {"9"        , Button::KEY_9},
    {"TAB"      , Button::KEY_TAB},
    {"Q"        , Button::KEY_Q},
    {"W"        , Button::KEY_W},
    {"E"        , Button::KEY_E},
    {"R"        , Button::KEY_R},
    {"T"        , Button::KEY_T},
    {"Y"        , Button::KEY_Y},
    {"U"        , Button::KEY_U},
    {"I"        , Button::KEY_I},
    {"O"        , Button::KEY_O},
    {"P"        , Button::KEY_P},
    {"A"        , Button::KEY_A},
    {"S"        , Button::KEY_S},
    {"D"        , Button::KEY_D},
    {"F"        , Button::KEY_F},
    {"G"        , Button::KEY_G},
    {"H"        , Button::KEY_H},
    {"J"        , Button::KEY_J},
    {"K"        , Button::KEY_K},
    {"L"        , Button::KEY_L},
    {"Z"        , Button::KEY_Z},
    {"X"        , Button::KEY_X},
    {"C"        , Button::KEY_C},
    {"V"        , Button::KEY_V},
    {"B"        , Button::KEY_B},
    {"N"        , Button::KEY_N},
    {"M"        , Button::KEY_M},
    {"SHIFT"    , Button::KEY_SHIFT},
    {"CTRL"     , Button::KEY_CTRL},
    {"ALT"      , Button::KEY_ALT},
    {"ESC"      , Button::KEY_ESC},
    {"F1"       , Button::KEY_F1},
    {"F2"       , Button::KEY_F2},
    {"F3"       , Button::KEY_F3},
    {"F4"       , Button::KEY_F4},
    {"F5"       , Button::KEY_F5},
    {"F6"       , Button::KEY_F6},
    {"F7"       , Button::KEY_F7},
    {"F8"       , Button::KEY_F8},
    {"F9"       , Button::KEY_F9},
    {"F10"      , Button::KEY_F10},
    {"F11"      , Button::KEY_F11},
    {"F12"      , Button::KEY_F12},
    {"UP"       , Button::KEY_UP},
    {"LEFT"     , Button::KEY_LEFT},
    {"DOWN"     , Button::KEY_DOWN},
    {"RIGHT"    , Button::KEY_RIGHT},
    {"SPACE"    , Button::KEY_SPACE},
    {"ENTER"    , Button::KEY_ENTER}
};
Button ModernBoy::Input::convert(
    const std::string& text
){
    auto upper = toUpper(text);
    auto it = text2button.find(upper);
    if (it == text2button.end()){
        return Button::KEY_UNKNOWN;
    }
    return it->second;
}

ButtonState ModernBoy::Input::transit(
    ButtonState state, uint8_t current
){
    uint8_t changed = ((state & ACTIVE_FLAG) ^ current) << 1;
    return static_cast<ButtonState>(changed | current);
}

State::State(){
    key.fill(ButtonState::None);
}

static std::string toUpper(const std::string& text){
    std::string upper = text;
    std::transform(text.begin(), text.end(), upper.begin(),
        [](unsigned char c){ return std::toupper(c); }
    );
    return upper;
}