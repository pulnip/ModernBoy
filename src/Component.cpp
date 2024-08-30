#include "Component.hpp"

using namespace ModernBoy;

Component::Component(Actor& actor)
: owner(actor){}

Component::~Component()=default;

void Component::update([[maybe_unused]] float dt){

}
