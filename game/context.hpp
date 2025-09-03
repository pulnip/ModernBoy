#ifndef MODERNBOY_GAME_CONTEXT_HPP
#define MODERNBOY_GAME_CONTEXT_HPP

#include <ranges>
#include "fwd.hpp"
#include "interface.hpp"
#include "entity_registry.hpp"
#include "component.hpp"
#include "action_system.hpp"
#include "camera_ray_system.hpp"
#include "control_system.hpp"
#include "game_command_bus.hpp"
#include "draw_system.hpp"
#include "input_system.hpp"
#include "physics_system.hpp"
#include "lifespan_system.hpp"
#include "intent_service.hpp"
#include "ray_service.hpp"
#include "scheduler.hpp"
#include "event.hpp"
#include "thread/generator.hpp"

namespace ModernBoy::Game
{
    struct ComponentInfo{
        ArchetypeBit bit;
        Index chunkIndex;
    };

    using EntityTable = std::unordered_map<EntityID, ComponentInfo>;

    class Context{
    public:
        Context(Engine& engine);

        void update(DeltaTime dt);

        uint32_t issueID();
        DeltaTime getDeltaTime();

        template<Event event>
        void on();

        DeltaTime getDeltaTime() const;

    private:
        void operator()(ActivateSystem);

    private:
        Engine& engine;

    public:
        EntityRegistry registry;

    private:
        GameCommandBus commandBus;
        IntentService playerIntent;
        IntentService editorIntent;
        RayService rayService;

        DrawSystem draw;
        ActionSystem action;
        CameraRaySystem camRay;
        ControlSystem control;
        InputSystem input;
        PhysicsSystem physics;
        LifespanSystem lifespan;
    };
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_CONTEXT_HPP