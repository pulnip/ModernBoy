#ifndef MODERNBOY_GAME_CAMERARAYSYSTEM_HPP
#define MODERNBOY_GAME_CAMERARAYSYSTEM_HPP

#include "engine/fwd.hpp"
#include "game_fwd.hpp"

namespace ModernBoy::Game
{
    class CameraRaySystem{
    public:
        CameraRaySystem(EntityRegistry&,
            ModernBoy::Service::InputService&, RayService&);

        void update(DeltaTime);

    private:
        EntityRegistry& registry;
        ModernBoy::Service::InputService& inputService;
        RayService& rayService;

    };
}

#endif // MODERNBOY_GAME_CAMERARAYSYSTEM_HPP
