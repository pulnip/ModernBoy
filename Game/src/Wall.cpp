#include "Component/BoxComponent.hpp"
#include "Component/Component.hpp"
#include "Component/MoveComponent.hpp"
#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GraphicsEngine.hpp"

#include "Wall.hpp"

void Wall::injectDependency() noexcept {
    auto self = weak_from_this();

    IComponent::make<MoveComponent>(self);
    auto bc = IComponent::make<BoxComponent>(self);

    bc->Observable<ColorRect>::subscribe(
        std::dynamic_pointer_cast<GraphicsEngine>(
            owner.lock()->query(SubEngineName::GraphicsEngine).value()
        )
    );
}

void Ceil::injectDependency() noexcept {
    // duplicated
    auto self = weak_from_this();

    auto mc=IComponent::make<MoveComponent>(self);
    auto bc = IComponent::make<BoxComponent>(self);

    bc->Observable<ColorRect>::subscribe(
        std::dynamic_pointer_cast<GraphicsEngine>(
            owner.lock()->query(SubEngineName::GraphicsEngine).value()
        )
    );
    // end

    mc->attr.set({
        {1024.0f/2, 15.0f/2},
        {1024.0f, 15.0f}
    });
    bc->setTexture({});
}

void Floor::injectDependency() noexcept {
    // duplicated
    auto self = weak_from_this();

    auto mc=IComponent::make<MoveComponent>(self);
    auto bc = IComponent::make<BoxComponent>(self);

    bc->Observable<ColorRect>::subscribe(
        std::dynamic_pointer_cast<GraphicsEngine>(
            owner.lock()->query(SubEngineName::GraphicsEngine).value()
        )
    );
    // end

    mc->attr.set({
        {1024.0f/2, 768.0f - 15.0f/2},
        {1024.0f, 15.0f}
    });
    bc->setTexture({});
}

void RightWall::injectDependency() noexcept {
    // duplicated
    auto self = weak_from_this();

    auto mc=IComponent::make<MoveComponent>(self);
    auto bc = IComponent::make<BoxComponent>(self);

    bc->Observable<ColorRect>::subscribe(
        std::dynamic_pointer_cast<GraphicsEngine>(
            owner.lock()->query(SubEngineName::GraphicsEngine).value()
        )
    );
    // end

    mc->attr.set({
        {1024.0f - 15.0f/2, 768.0f/2},
        {15.0f, 768.0f}
    });
    bc->setTexture({});
}
