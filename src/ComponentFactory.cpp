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

using namespace std;
using namespace ModernBoy;

shared_ptr<Component> ComponentFactory::make(
    ComponentType componentType, Actor& actor
){
    using enum ComponentType;

    switch(componentType){
    case NIL_COMPONENT:
        return make_shared<Component>(actor);
    case UNDEFINED_COMPONENT:
        [[fallthrough]];
    default:
        throw NotImplemented();
    }
}

