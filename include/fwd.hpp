#ifndef MODERNBOY_FWD_HPP
#define MODERNBOY_FWD_HPP

#include <cstdint>

namespace ModernBoy
{
    struct AppState;
    class UI;

    namespace Input
    {
        class Controller;
    }

    using ObserverID = uint32_t;
}

#endif // MODERNBOY_FWD_HPP