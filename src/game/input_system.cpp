#include "entity_registry.hpp"
#include "input_system.hpp"
#include "engine/service/input_service.hpp"
#include "intent_service.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;
using namespace ModernBoy::Service;
using namespace ModernBoy::Input;

PlayerInputSystem::PlayerInputSystem(
    EntityRegistry& registry,
    InputService& inputSrv,
    IntentService& intentSrv)
:registry(registry), inputService(inputSrv)
,intentService(intentSrv){}

EditorInputSystem::EditorInputSystem(
    EntityRegistry& registry,
    InputService& inputSrv,
    IntentService& intentSrv)
:registry(registry), inputService(inputSrv)
,intentService(intentSrv){}

InputSystem::InputSystem(EntityRegistry& registry,
    InputService& inputSrv,
    IntentService& playerItt, IntentService& editorItt)
:playerInputSystem(registry, inputSrv, playerItt)
,editorInputSystem(registry, inputSrv, editorItt){}


void InputSystem::update(DeltaTime deltaTime){
    playerInputSystem.update(deltaTime);
    editorInputSystem.update(deltaTime);
}

void PlayerInputSystem::update(DeltaTime deltaTime){
    auto dt = deltaTime.count() / 1'000'000.0f;
    auto input = inputService.snapshot();

    for(auto [_1, _2, _3]: registry.query<Player>()){
        float dx=0, dz=0;

        if(input.keyboard[KEY_A] == Held)
            dx -= 10;
        if(input.keyboard[KEY_D] == Held)
            dx += 10;
        if(input.keyboard[KEY_W] == Held)
            dz += 10;
        if(input.keyboard[KEY_S] == Held)
            dx -= 10;

        if(dx != 0 || dz != 0){
            intentService.write(MoveIntent{.move=Vec3{
                .x=dx, .y=0, .z=dz
            }});
        }

        Vec2 mouse_move{
            .x = input.mouse.dx,
            .y = input.mouse.dy
        };
        if(norm_squared(mouse_move) > 0){
            auto axis = normalize(Vec3{
                .x = input.mouse.dy,
                .y = input.mouse.dx,
                .z = 0
            });
            float theta = norm(mouse_move) / 10.0f;

            intentService.write(LookIntent{
                axis, theta
            });
        }
    }
}

void EditorInputSystem::update(DeltaTime deltaTime){
    auto dt = deltaTime.count() / 1'000'000.0f;
    auto input = inputService.snapshot();

    for(auto [_1, _2, _3]: registry.query<Editor>()){
        float dx=0, dy=0, dz=0;

        if(input.keyboard[KEY_A] == Held)
            dx -= 10;
        if(input.keyboard[KEY_D] == Held)
            dx += 10;
        if(input.keyboard[KEY_SHIFT] == Held)
            dy -= 10;
        if(input.keyboard[KEY_SPACE] == Held)
            dy += 10;
        if(input.keyboard[KEY_W] == Held)
            dz += 10;
        if(input.keyboard[KEY_S] == Held)
            dz -= 10;

        if(dx != 0 || dz != 0){
            intentService.write(MoveIntent{.move=Vec3{
                .x=dx, .y=dy, .z=dz
            }});
        }

        Vec2 mouse_move{
            .x = input.mouse.dx,
            .y = input.mouse.dy
        };
        if(norm_squared(mouse_move) > 0){
            auto axis = normalize(Vec3{
                .x = input.mouse.dy,
                .y = input.mouse.dx,
                .z = 0
            });
            float theta = norm(mouse_move) / 10.0f;

            intentService.write(LookIntent{
                axis, theta
            });
        }
    }
}
