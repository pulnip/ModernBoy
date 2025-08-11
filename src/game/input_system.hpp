#ifndef MODERNBOY_GAME_INPUT_SYSTEM_HPP
#define MODERNBOY_GAME_INPUT_SYSTEM_HPP

#include "engine/fwd.hpp"
#include "game_fwd.hpp"
#include "engine/interface/debug_draw_service.hpp"

namespace ModernBoy::Game
{
    class PlayerInputSystem{
    public:
        PlayerInputSystem(
            Interface::InputService&,
            Interface::EngineCommandBus&,
            EntityRegistry&,
            IntentService&);

        void update(DeltaTime);

    private:
        Interface::InputService& inputService;
        Interface::EngineCommandBus& commandBus;
        EntityRegistry& registry;
        IntentService& intentService;
    };

    class EditorInputSystem{
    public:
        EditorInputSystem(
            Interface::InputService&,
            Interface::EngineCommandBus&,
            EntityRegistry&,
            IntentService&);

        void update(DeltaTime);

    private:
        Interface::InputService& inputService;
        Interface::EngineCommandBus& commandBus;
        EntityRegistry& registry;
        IntentService& intentService;
    };

    class InputSystem{
    public:
        InputSystem(
            Interface::InputService&,
            Interface::EngineCommandBus&,
            EntityRegistry&,
            IntentService&, IntentService&);

        void update(DeltaTime);

    private:
        PlayerInputSystem playerInputSystem;
        EditorInputSystem editorInputSystem;
    };
}

#endif // MODERNBOY_GAME_INPUT_SYSTEM_HPP