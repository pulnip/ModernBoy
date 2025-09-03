#include "entity_registry.hpp"
#include "interface/engine_command_bus.hpp"
#include "interface/input_service.hpp"
#include "game_command_bus.hpp"
#include "input_system.hpp"
#include "intent_service.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;
using namespace ModernBoy::Interface;
using namespace ModernBoy::Input;

PlayerInputSystem::PlayerInputSystem(
    InputService& inputSrv, EngineCommandBus& bus,
    GameCommandBus& gameCommandBus,
    EntityRegistry& registry, IntentService& intentSrv)
:inputService(inputSrv), commandBus(bus), gameCommandBus(gameCommandBus)
,registry(registry), intentService(intentSrv){}

EditorInputSystem::EditorInputSystem(
    InputService& inputSrv, EngineCommandBus& bus,
    GameCommandBus& gameCommandBus,
    EntityRegistry& registry, IntentService& intentSrv)
:inputService(inputSrv), commandBus(bus), gameCommandBus(gameCommandBus)
,registry(registry), intentService(intentSrv){}

InputSystem::InputSystem(
    InputService& inputSrv, EngineCommandBus& bus,
    GameCommandBus& gameCommandBus, EntityRegistry& registry,
    IntentService& playerItt, IntentService& editorItt)
:playerInputSystem(inputSrv, bus, gameCommandBus, registry, playerItt)
,editorInputSystem(inputSrv, bus, gameCommandBus, registry, editorItt){}

void InputSystem::update(){
    playerInputSystem.update();
    editorInputSystem.update();
}

void PlayerInputSystem::update(){
    const auto& input = inputService.snapshot();

    if(input.keyboard[KEY_ALT] == Pressed){
        isUIMode = !isUIMode;

        commandBus.write(SetMouseMode{
            .isRelative = !isUIMode
        });
        gameCommandBus.write(ActivateSystem{
            .targetSystem = CAMERA_RAY,
            .activate = isUIMode
        });
    }

    for(auto [_1, _2, _3]: registry.query<Player>()){
        float dx=0, dz=0;

        if(input.keyboard[KEY_A] == Held)
            dx -= 1;
        if(input.keyboard[KEY_D] == Held)
            dx += 1;
        if(input.keyboard[KEY_W] == Held)
            dz += 1;
        if(input.keyboard[KEY_S] == Held)
            dz -= 1;

        if(dx != 0 || dz != 0){
            intentService.write(MoveIntent{.move=Vec3{
                .x=dx, .y=0, .z=dz
            }});
        }

        if(!isUIMode && (input.mouse.pos0 != input.mouse.pos)){
            intentService.write(LookIntent{
                .yaw = input.mouse.dpos.x,
                .pitch = input.mouse.dpos.y
            });
        }

        if(!isUIMode && norm_squared(input.mouse.dpos) > 1e-6)
            intentService.write(LookIntent{
                .yaw = input.mouse.dpos.x,
                .pitch = input.mouse.dpos.y
            });
    }
}

void EditorInputSystem::update(){
    const auto& input = inputService.snapshot();

    if(input.keyboard[KEY_ALT] == Pressed){
        isUIMode = !isUIMode;

        commandBus.write(SetMouseMode{
            .isRelative = !isUIMode
        });
        gameCommandBus.write(ActivateSystem{
            .targetSystem = CAMERA_RAY,
            .activate = isUIMode
        });
    }

    for(auto [_1, _2, _3]: registry.query<Editor>()){
        float dx=0, dy=0, dz=0;

        if(input.keyboard[KEY_A] == Held)
            dx -= 1;
        if(input.keyboard[KEY_D] == Held)
            dx += 1;
        if(input.keyboard[KEY_SHIFT] == Held)
            dy -= 1;
        if(input.keyboard[KEY_SPACE] == Held)
            dy += 1;
        if(input.keyboard[KEY_W] == Held)
            dz += 1;
        if(input.keyboard[KEY_S] == Held)
            dz -= 1;

        if(dx != 0 || dy != 0 || dz != 0){
            intentService.write(MoveIntent{.move=Vec3{
                .x=dx, .y=dy, .z=dz
            }});
        }

        if(!isUIMode && norm_squared(input.mouse.dpos) > 1e-6)
            intentService.write(LookIntent{
                .yaw = input.mouse.dpos.x,
                .pitch = input.mouse.dpos.y
            });
    }
}
