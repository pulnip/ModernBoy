#ifndef MODERNBOY_GAME_INPUT_SYSTEM_HPP
#define MODERNBOY_GAME_INPUT_SYSTEM_HPP

#include "engine/fwd.hpp"
#include "game_fwd.hpp"

namespace ModernBoy::Game
{
    class InputSystem{
    public:
        InputSystem(EntityRegistry&,
            ModernBoy::Input::Device&,
            ModernBoy::Render::Renderer&);

        void update(DeltaTime);

    private:
        EntityRegistry& registry;
        ModernBoy::Input::Device& input;
        ModernBoy::Render::Renderer& render;
    };
}

#endif // MODERNBOY_GAME_INPUT_SYSTEM_HPP