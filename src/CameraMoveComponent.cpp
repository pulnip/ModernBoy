#include <print>
#include <directxtk/SimpleMath.h>
#include <SDL2/SDL_keycode.h>
#include "Actor.hpp"
#include "CameraMoveComponent.hpp"
#include "InputSystem.hpp"

using namespace std;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

CameraMoveComponent::CameraMoveComponent(Actor& actor)
: InputComponent(actor){}


void CameraMoveComponent::onPressed(const InputState& state){
    constexpr float vel=5.0f;

    switch(state.btn.keyType){
    case SDLK_a:
        xVel=-vel;
        break;
    case SDLK_d:
        xVel=vel;
        break;
    case SDLK_e:
        zVel=vel;
        break;
    case SDLK_q:
        zVel=-vel;
        break;
    case SDLK_s:
        yVel=-vel;
        break;
    case SDLK_w:
        yVel=vel;
        break;
    }
}

void CameraMoveComponent::onReleased([[maybe_unused]] const InputState& state){
    switch(state.btn.keyType){
    case SDLK_a:
        [[fallthrough]];
    case SDLK_d:
        xVel=0;
        break;
    case SDLK_e:
        [[fallthrough]];
    case SDLK_q:
        zVel=0;
        break;
    case SDLK_s:
        [[fallthrough]];
    case SDLK_w:
        yVel=0;
        break;
    }
}

void CameraMoveComponent::update(float dt){
    owner.transform.position += dt * Vector3(xVel, yVel, zVel);
}
