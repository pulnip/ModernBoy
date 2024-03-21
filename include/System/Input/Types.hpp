#pragma once

namespace Input{
    enum class Key{
        // keyboard event
        KB_ARROW_DOWN,
        KB_ARROW_LEFT,
        KB_ARROW_RIGHT,
        KB_ARROW_UP,
        KB_DEL,
        KB_END,
        KB_ENTER,
        KB_ESCAPE,
        KB_HOME,
        KB_INSERT,
        KB_LEFT_ALT,
        KB_LEFT_CTRL,
        KB_LEFT_SHIFT,
        KB_PAGE_UP,
        KB_PAGE_DOWN,
        KB_RIGHT_ALT,
        KB_RIGHT_CTRL,
        KB_RIGHT_SHIFT,
        KB_SPACE,
        KB_A='a',
        KB_D='d',
        KB_Q='q',
        KB_S='s',
        KB_W='w',

        // mouse event
        MOUSE_LEFT_CLICK,
        MOUSE_RIGHT_CLICK,
        MOUSE_MOVE
    };

    struct Event{
        enum class Status{
            PRESSED,
            RELEASED
        } status;
        Key key;
    };
}