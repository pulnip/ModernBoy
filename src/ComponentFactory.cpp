#include "Actor.hpp"
#include "CameraComponent.hpp"
#include "CameraMoveComponent.hpp"
#include "Core.hpp"
#include "Component.hpp"
#include "ComponentFactory.hpp"
#include "CubeMeshComponent.hpp"
#include "CylinderMeshComponent.hpp"
#include "GridMeshComponent.hpp"
#include "LightComponent.hpp"
#include "MyException.hpp"
#include "SphereMeshComponent.hpp"
#include "TriangleMeshComponent.hpp"
#include "Window.hpp"
#include "RenderAdaptor.hpp"

using namespace std;
using namespace ModernBoy;

shared_ptr<Component> ComponentFactory::make(
    ComponentType componentType, Actor& actor
){
    using enum ComponentType;

    shared_ptr<Component> component;

    switch(componentType){
    case NIL_COMPONENT:
        return make_shared<Component>(actor);
    case CUBE_MESH: {
        component=make_shared<CubeMeshComponent>(
            actor, core.window->renderer->device,
            "assets/crate.png"
        );
    } break;
    case GRID_MESH: {
        component=make_shared<GridMeshComponent>(
            actor, core.window->renderer->device,
            5, 3,
            "assets/crate.png"
        );
    } break;
    case CYLINDER_MESH: {
        component=make_shared<CylinderMeshComponent>(
            actor, core.window->renderer->device,
            20, 5,
            "assets/crate.png"
        );
    } break;
    case SPHERE_MESH: {
        component=make_shared<SphereMeshComponent>(
            actor, core.window->renderer->device,
            40, 15,
            "assets/ojwD8.jpg"
        );
    } break;
    case TRIANGLE_MESH:
    case DEFAULT_LIGHT:
    case MAIN_CAMERA:
    case CAMERA_MOVE:
    case UNDEFINED_COMPONENT:
        [[fallthrough]];
    default:
        throw NotImplemented();
    }

    switch(componentType){
    case CUBE_MESH:
        [[fallthrough]];
    case GRID_MESH:
        [[fallthrough]];
    case CYLINDER_MESH:
        [[fallthrough]];
    case SPHERE_MESH:
        [[fallthrough]];
    case TRIANGLE_MESH: {
        core.window->addMesh(dynamic_pointer_cast<MeshComponent>(component));
    } break;
    default:
        throw NotImplemented();
    }

    return component;
}

