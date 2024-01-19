#include <cassert>

#include "ActorManagerForTest.hpp"
#include "Paddle.hpp"

void ActorManagerForTest::injectDependency() noexcept{
    auto self=std::dynamic_pointer_cast<ActorManager>(shared_from_this());
    assert(self!=nullptr);

    auto paddle=Actor::make<Paddle>(self);
}
