#include "Wall.hpp"
#include "ActorManager.hpp"
#include "BoxComponent.hpp"
#include "Component.hpp"
#include "GraphicsEngine.hpp"
#include "MoveComponent.hpp"

Wall::Wall(const std::weak_ptr<ActorManager> owner) noexcept
    : Actor(owner) {
}
void Wall::postConstruct() noexcept {
    auto self = weak_from_this();

    bc = Component::make<BoxComponent>(self);
    mc = Component::make<MoveComponent>(self);

    bc->Observable<ColorRect>::subscribe(std::dynamic_pointer_cast<GraphicsEngine>(
        owner.lock()->requestSubEngine(SubEngineName::GraphicsEngine).value().lock()));
}