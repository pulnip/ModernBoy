#ifndef MODERNBOY_INPUT_STATE_HPP
#define MODERNBOY_INPUT_STATE_HPP

#include <array>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL_keycode.h>
#include "common/type.hpp"
#include "fwd.hpp"

namespace ModernBoy::Input
{
    constexpr uint8_t ACTIVE_FLAG  = 0b01;
    constexpr uint8_t CHANGED_FLAG = 0b10;

    enum ButtonState{
        None        = 0b00,
        Held        = ACTIVE_FLAG,
        Released    = CHANGED_FLAG,
        Pressed     = CHANGED_FLAG | ACTIVE_FLAG,
        STATE_INVALID = 4,
    };
    ButtonState toButtonState(const std::string& text);
    std::string toText(ButtonState state);

    enum Button{
        KEY_0     =  0,
        KEY_1     =  1,
        KEY_2     =  2,
        KEY_3     =  3,
        KEY_4     =  4,
        KEY_5     =  5,
        KEY_6     =  6,
        KEY_7     =  7,
        KEY_8     =  8,
        KEY_9     =  9,
        KEY_TAB   = 10,
        KEY_Q     = 11,
        KEY_W     = 12,
        KEY_E     = 13,
        KEY_R     = 14,
        KEY_T     = 15,
        KEY_Y     = 16,
        KEY_U     = 17,
        KEY_I     = 18,
        KEY_O     = 19,
        KEY_P     = 20,
        KEY_A     = 21,
        KEY_S     = 22,
        KEY_D     = 23,
        KEY_F     = 24,
        KEY_G     = 25,
        KEY_H     = 26,
        KEY_J     = 27,
        KEY_K     = 28,
        KEY_L     = 29,
        KEY_Z     = 30,
        KEY_X     = 31,
        KEY_C     = 32,
        KEY_V     = 33,
        KEY_B     = 34,
        KEY_N     = 35,
        KEY_M     = 36,
        KEY_SHIFT = 37,
        KEY_CTRL  = 38,
        KEY_ALT   = 39,
        KEY_ESC   = 40,
        KEY_F1    = 41,
        KEY_F2    = 42,
        KEY_F3    = 43,
        KEY_F4    = 44,
        KEY_F5    = 45,
        KEY_F6    = 46,
        KEY_F7    = 47,
        KEY_F8    = 48,
        KEY_F9    = 49,
        KEY_F10   = 50,
        KEY_F11   = 51,
        KEY_F12   = 52,
        KEY_UP    = 53,
        KEY_LEFT  = 54,
        KEY_DOWN  = 55,
        KEY_RIGHT = 56,
        KEY_SPACE = 57,
        KEY_ENTER = 58,
        KEY_UNKNOWN = 59,
    };
    bool isValid(Button button);
    bool isValid(ButtonState state);

    SDL_Scancode convert(Button code);
    Button convert(SDL_Scancode code);
    Button toButton(const std::string& text);
    std::string toText(Button button);

    ButtonState transit(ButtonState state,
        uint8_t current);

    using KeyState = std::array<ButtonState, KEY_UNKNOWN>;
    
    struct State{
        KeyState keyState;
    };

    constexpr uint32_t STATE_MASK = 0b11;
    constexpr uint32_t BUTTON_MASK = std::numeric_limits<uint32_t>::max() - STATE_MASK;

    struct Trigger{
        Button button;
        ButtonState onState;
    }; static_assert(std::is_trivially_copyable_v<Trigger>);
}

#endif // MODERNBOY_INPUT_STATE_HPP