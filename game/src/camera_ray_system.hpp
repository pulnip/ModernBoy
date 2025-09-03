#ifndef MODERNBOY_GAME_CAMERARAYSYSTEM_HPP
#define MODERNBOY_GAME_CAMERARAYSYSTEM_HPP

#include "fwd.hpp"
#include "game_fwd.hpp"

namespace ModernBoy::Game
{
    class CameraRaySystem{
    public:
        CameraRaySystem(EntityRegistry&,
            ModernBoy::Interface::InputService&, RayService&);

        void update();

        void setActive(bool activate);

    private:
        bool isActive = false;

        EntityRegistry& registry;
        ModernBoy::Interface::InputService& inputService;
        RayService& rayService;
    };
}

#endif // MODERNBOY_GAME_CAMERARAYSYSTEM_HPP
