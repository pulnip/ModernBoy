#include <print>
#include <directxtk/SimpleMath.h>
#include <SDL2/SDL_keycode.h>
#include "Actor.hpp"
#include "CameraMoveComponent.hpp"
#include "InputSystem.hpp"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

CameraMoveComponent::CameraMoveComponent(Actor& actor)
: InputComponent(actor){}


void CameraMoveComponent::onPressed(const InputState& state){
    constexpr float vel=5.0f;
    constexpr float rot=XM_PIDIV2;

    switch(state.btn.keyType){
    case SDLK_a:
        xVel=-vel;
        break;
    case SDLK_d:
        xVel=vel;
        break;
    case SDLK_s:
        yVel=-vel;
        break;
    case SDLK_w:
        yVel=vel;
        break;
    case SDLK_f:
        zVel=-vel;
        break;
    case SDLK_r:
        zVel=vel;
        break;
    case SDLK_q:
        yRot=-rot;
        break;
    case SDLK_e:
        yRot=rot;
        break;
    }
}

void CameraMoveComponent::onReleased(const InputState& state){
    switch(state.btn.keyType){
    case SDLK_a:
        [[fallthrough]];
    case SDLK_d:
        xVel=0.0f;
        break;
    case SDLK_s:
        [[fallthrough]];
    case SDLK_w:
        yVel=0.0f;
        break;
    case SDLK_f:
        [[fallthrough]];
    case SDLK_r:
        zVel=0.0f;
        break;
    case SDLK_q:
        [[fallthrough]];
    case SDLK_e:
        yRot=0.0f;
        break;
    }
}

void CameraMoveComponent::update(float dt){
    owner.transform.position += dt * Vector3(xVel, yVel, zVel);
    auto addend=Quaternion::CreateFromAxisAngle(Vector3::UnitY, dt*yRot);
    owner.transform.quaternion=XMQuaternionMultiply(owner.transform.quaternion, addend);
}
