#ifndef MODERNBOY_UI_EVENT_HPP
#define MODERNBOY_UI_EVENT_HPP

#include <cstdint>

namespace ModernBoy::UI
{
    using ControlID = uint32_t;

    enum class EventType{
        OnPressed,
        OnReleased,
        OnClicked,
        OnDoubleClicked,
        OnHovered,
        OnUnhovered,
        OnFocused,
        OnUnfocused,
        OnKeyDown,
        OnKeyUp,
        OnTextInput,
        StateChanged,
        OnShown,
        OnHidden,
        OnResized,
        OnSubmit,
        OnCancelled,
        OnError,
        Custom
    };

    struct Event{
        ControlID id;
        EventType type;
        union{
            struct{ bool checked; };
            struct{ int index; };
            struct{ float value; };
        };
    };
} // namespace ModernBoy::UI

#endif // MODERNBOY_UI_EVENT_HPP