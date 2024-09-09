#include <source_location>
#include "Actor.hpp"
#include "ActorFactory.hpp"
#include "ComponentFactory.hpp"
#include "MyException.hpp"

using namespace std;
using namespace ModernBoy;

shared_ptr<Actor> ActorFactory::make(ActorType actorType){
    using enum ActorType;
    using enum ComponentType;

    auto actor=make_shared<Actor>(core);
    ComponentFactory compMaker(core);

    switch(actorType){
        case NIL:
            break;
        case CAMERA: {
            actor->addComponent(compMaker.make(MAIN_CAMERA, *actor));
            actor->addComponent(compMaker.make(CAMERA_MOVE, *actor));
        } break;
        case LIGHT: {
            actor->addComponent(compMaker.make(DEFAULT_LIGHT, *actor));
            actor->addComponent(compMaker.make(DEFAULT_LIGHT, *actor));
            actor->addComponent(compMaker.make(DEFAULT_LIGHT, *actor));
        } break;
        case TEXTURE_CUBE: {
            actor->addComponent(compMaker.make(CUBE_MESH, *actor));
        } break;
        case TEXTURE_GRID: {
            actor->addComponent(compMaker.make(GRID_MESH, *actor));
        } break;
        case TEXTURE_CYLINDER: {
            actor->addComponent(compMaker.make(CYLINDER_MESH, *actor));
        } break;
        case TEXTURE_SPHERE: {
            actor->addComponent(compMaker.make(SPHERE_MESH, *actor));
        } break;
        case UNDEFINED:
            [[fallthrough]];
        default:
            throw NotImplemented();
    }

    return actor;
}
