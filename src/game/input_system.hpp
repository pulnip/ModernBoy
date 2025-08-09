#ifndef MODERNBOY_GAME_INPUT_SYSTEM_HPP
#define MODERNBOY_GAME_INPUT_SYSTEM_HPP

#include "engine/fwd.hpp"
#include "game_fwd.hpp"
#include "engine/service/debug_draw_service.hpp"

namespace ModernBoy::Game
{
    class PlayerInputSystem{
    public:
        PlayerInputSystem(EntityRegistry&,
            Service::InputService&,
            IntentService&);

        void update(DeltaTime);

    private:
        EntityRegistry& registry;
        Service::InputService& inputService;
        IntentService& intentService;
    };

    class EditorInputSystem{
    public:
        EditorInputSystem(EntityRegistry&,
            Service::InputService&,
            IntentService&);

        void update(DeltaTime);

    private:
        EntityRegistry& registry;
        Service::InputService& inputService;
        IntentService& intentService;
    };

    class InputSystem{
    public:
        InputSystem(EntityRegistry&,
            Service::InputService&,
            IntentService&, IntentService&);

        void update(DeltaTime);

    private:
        PlayerInputSystem playerInputSystem;
        EditorInputSystem editorInputSystem;
    };
}

#endif // MODERNBOY_GAME_INPUT_SYSTEM_HPP