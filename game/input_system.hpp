#ifndef MODERNBOY_GAME_INPUT_SYSTEM_HPP
#define MODERNBOY_GAME_INPUT_SYSTEM_HPP

#include "fwd.hpp"
#include "game_fwd.hpp"
#include "interface/draw_service.hpp"

namespace ModernBoy::Game
{
    class PlayerInputSystem{
    public:
        PlayerInputSystem(
            Interface::InputService&,
            Interface::EngineCommandBus&,
            GameCommandBus&,
            EntityRegistry&,
            IntentService&);

        void update();

    private:
        Interface::InputService& inputService;
        Interface::EngineCommandBus& commandBus;
        GameCommandBus& gameCommandBus;
        EntityRegistry& registry;
        IntentService& intentService;

        bool isUIMode = false;
    };

    class EditorInputSystem{
    public:
        EditorInputSystem(
            Interface::InputService&,
            Interface::EngineCommandBus&,
            GameCommandBus&,
            EntityRegistry&,
            IntentService&);

        void update();

    private:
        Interface::InputService& inputService;
        Interface::EngineCommandBus& commandBus;
        GameCommandBus& gameCommandBus;
        EntityRegistry& registry;
        IntentService& intentService;

        bool isUIMode = false;
    };

    class InputSystem{
    public:
        InputSystem(
            Interface::InputService&,
            Interface::EngineCommandBus&,
            GameCommandBus&,
            EntityRegistry&,
            IntentService&, IntentService&);

        void update();

    private:
        PlayerInputSystem playerInputSystem;
        EditorInputSystem editorInputSystem;
    };
}

#endif // MODERNBOY_GAME_INPUT_SYSTEM_HPP