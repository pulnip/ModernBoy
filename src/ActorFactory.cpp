#include <source_location>
#include "Actor.hpp"
#include "ActorFactory.hpp"
#include "ActorManager.hpp"
#include "CameraComponent.hpp"
#include "CameraMoveComponent.hpp"
#include "Core.hpp"
#include "CubeMeshComponent.hpp"
#include "CylinderMeshComponent.hpp"
#include "GridMeshComponent.hpp"
#include "helper.hpp"
#include "InputSystem.hpp"
#include "LightComponent.hpp"
#include "RenderAdaptor.hpp"
#include "SphereMeshComponent.hpp"
#include "TriangleMeshComponent.hpp"
#include "Window.hpp"

using namespace std;
using namespace ModernBoy;

class NotImplemented: public Exception{
  public:
    NotImplemented(const source_location& sl=source_location::current())
    : Exception("Not Implemented", sl){}
};

shared_ptr<Actor> ActorFactory::make(ActorType actorType, Core& core){
    using enum ActorType;

    auto actor=make_shared<Actor>(core);

    switch(actorType){
        case NIL:
            break;
        case CAMERA: {
            auto camera=make_shared<CameraComponent>(*actor);
            auto control=make_shared<CameraMoveComponent>(*actor);
            actor->addComponent(camera);
            actor->addComponent(control);
            camera->setScreenSize(core.window->getSize());
            actor->transform.position=-5.0f*Vector3::UnitZ;
            core.window->setMainCamera(camera);
            core.inputSystem->subscribe(control);
        } break;
        case LIGHT: {
            auto dirLight=make_shared<LightComponent>(*actor);
            auto pointLight=make_shared<LightComponent>(*actor);
            auto spotLight=make_shared<LightComponent>(*actor);
            actor->addComponent(dirLight);
            actor->addComponent(pointLight);
            actor->addComponent(spotLight);
            core.window->addLight(dirLight);
            core.window->addLight(pointLight);
            core.window->addLight(spotLight);
        } break;
        case TEXTURE_CUBE: {
            auto mesh=make_shared<CubeMeshComponent>(
                *actor, core.window->renderer->device,
                "assets/crate.png"
            );
            actor->addComponent(mesh);
            core.window->addMesh(mesh);
        } break;
        case TEXTURE_GRID: {
            auto mesh=make_shared<GridMeshComponent>(
                *actor, core.window->renderer->device,
                5, 3,
                "assets/crate.png"
            );
            actor->addComponent(mesh);
            core.window->addMesh(mesh);
        } break;
        case TEXTURE_CYLINDER: {
            auto mesh=make_shared<CylinderMeshComponent>(
                *actor, core.window->renderer->device,
                20, 5,
                "assets/crate.png"
            );
            actor->addComponent(mesh);
            core.window->addMesh(mesh);
        } break;
        case TEXTURE_SPHERE: {
            auto mesh=make_shared<SphereMeshComponent>(
                *actor, core.window->renderer->device,
                40, 15,
                "assets/ojwD8.jpg"
            );
            actor->addComponent(mesh);
            core.window->addMesh(mesh);
        } break;
        case UNDEFINED:
            [[fallthrough]];
        default:
            throw NotImplemented();
    }

    return actor;
}
