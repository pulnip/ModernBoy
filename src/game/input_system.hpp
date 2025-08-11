#ifndef MODERNBOY_GAME_INPUT_SYSTEM_HPP
#define MODERNBOY_GAME_INPUT_SYSTEM_HPP

#include "engine/fwd.hpp"
#include "game_fwd.hpp"
#include "engine/interface/debug_draw_service.hpp"

namespace ModernBoy::Game
{
    class PlayerInputSystem{
    public:
        PlayerInputSystem(EntityRegistry&,
            Interface::InputService&,
            IntentService&);

        void update(DeltaTime);

    private:
        EntityRegistry& registry;
        Interface::InputService& inputService;
        IntentService& intentService;
    };

    class EditorInputSystem{
    public:
        EditorInputSystem(EntityRegistry&,
            Interface::InputService&,
            IntentService&);

        void update(DeltaTime);

    private:
        EntityRegistry& registry;
        Interface::InputService& inputService;
        IntentService& intentService;
    };

    class InputSystem{
    public:
        InputSystem(EntityRegistry&,
            Interface::InputService&,
            IntentService&, IntentService&);

        void update(DeltaTime);

    private:
        PlayerInputSystem playerInputSystem;
        EditorInputSystem editorInputSystem;
    };
}

#endif // MODERNBOY_GAME_INPUT_SYSTEM_HPP