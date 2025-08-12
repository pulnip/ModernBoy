#ifndef MODERNBOY_GAME_CONTEXT_HPP
#define MODERNBOY_GAME_CONTEXT_HPP

#include <ranges>
#include "engine/fwd.hpp"
#include "engine/interface.hpp"
#include "entity_registry.hpp"
#include "component.hpp"
#include "action_system.hpp"
#include "camera_ray_system.hpp"
#include "control_system.hpp"
#include "debug_system.hpp"
#include "game_command_bus.hpp"
#include "draw_system.hpp"
#include "input_system.hpp"
#include "physics_system.hpp"
#include "lifespan_system.hpp"
#include "intent_service.hpp"
#include "ray_service.hpp"
#include "scheduler.hpp"
#include "event.hpp"
#include "core/thread/generator.hpp"

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

        template<typename Task>
        const std::vector<Task>& getBuffer() const;

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

        DebugSystem debug;
        DrawSystem draw;
        ActionSystem action;
        CameraRaySystem camRay;
        ControlSystem control;
        InputSystem input;
        PhysicsSystem physics;
        LifespanSystem lifespan;

        friend class ModernBoy::AssetLoader;
    };
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_CONTEXT_HPP