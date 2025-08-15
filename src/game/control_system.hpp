#ifndef MODERNBOY_GAME_CONTROL_SYSTEM_HPP
#define MODERNBOY_GAME_CONTROL_SYSTEM_HPP

#include "engine/fwd.hpp"
#include "game_fwd.hpp"
#include "intent_service.hpp"

namespace ModernBoy::Game
{
    class PlayerControlSystem{
    public:
        PlayerControlSystem(
            EntityRegistry&, IntentService&);

        void update(DeltaTime);

    private:
        EntityRegistry& registry;
        IntentService& service;
};

    class EditorControlSystem{
    public:
        EditorControlSystem(
            EntityRegistry&, IntentService&);

        void update(DeltaTime);

    private:
        EntityRegistry& registry;
        IntentService& service;
 };

    class ControlSystem{
    public:
        ControlSystem(EntityRegistry&,
            IntentService& playerSrv, IntentService& editorSrv);

        void update(DeltaTime);

    private:
        PlayerControlSystem playerControlSystem;
        EditorControlSystem editorControlSystem;
    };
}

#endif // MODERNBOY_GAME_CONTROL_SYSTEM_HPP