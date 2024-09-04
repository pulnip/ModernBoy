#include "Actor.hpp"
#include "Component.hpp"
#include "ComponentFactory.hpp"
#include "helper.hpp"

using namespace std;
using namespace ModernBoy;

shared_ptr<Component> ComponentFactory::make(
    ComponentType componentType, Actor& actor
){
    using enum ComponentType;

    switch(componentType){
    case NIL_COMPONENT:
        break;
    case UNDEFINED_COMPONENT:
        [[fallthrough]];
    default:
        throw;
    }
}

