#include <source_location>
#include "Actor.hpp"
#include "ActorFactory.hpp"
#include "helper.hpp"

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
        case UNDEFINED:
            [[fallthrough]];
        default:
            throw NotImplemented();
    }

    return actor;
}
