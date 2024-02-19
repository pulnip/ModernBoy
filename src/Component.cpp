#include <cassert>

#include "Actor/Actor.hpp"
#include "Component/Component.hpp"

using namespace Game::Component;

void Interface::postConstruct() noexcept{
    assert(not owner.expired());
    owner.lock()->add(shared_from_this());

}