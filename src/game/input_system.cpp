#include "entity_registry.hpp"
#include "input_system.hpp"
#include "engine/interface/input_service.hpp"
#include "intent_service.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;
using namespace ModernBoy::Interface;
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

inline auto mouseToSphere(Vec2 mpos){
    auto d2 = norm_squared(mpos);
    if(d2 <= 1.0f){
        auto z = std::sqrt(std::max(0.0f, 1.0f - d2));
        return normalize(asVec3(mpos, z));
    }
    else{
        auto d = std::sqrt(d2);
        auto z = 1.0f / (2.0f * d);
        return normalize(asVec3(z/d * mpos, z));
    }
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
            dz -= 10;

        if(dx != 0 || dz != 0){
            intentService.write(MoveIntent{.move=Vec3{
                .x=dx, .y=0, .z=dz
            }});
        }

        if(input.mouse.pos0 != input.mouse.pos){
            intentService.write(LookIntent{
                .yaw = input.mouse.dpos.x,
                .pitch = input.mouse.dpos.y
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

        if(input.mouse.pos0 != input.mouse.pos){
            intentService.write(LookIntent{
                .yaw = input.mouse.dpos.x,
                .pitch = input.mouse.dpos.y
            });
        }
    }
}
