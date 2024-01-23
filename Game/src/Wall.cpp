#include "Skin.hpp"
#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "Wall.hpp"
#include "Component/BoxComponent.hpp"
#include "Component/MoveComponent.hpp"

void Wall::injectDependency() noexcept {
    auto self = weak_from_this();

    Component::make<MoveComponent>(self);
    auto bc = Component::make<BoxComponent>(self);

    bc->UniqueObservable<ColorRect>::subscribe(
        std::dynamic_pointer_cast<GraphicsEngine>(
            owner.lock()->query(SubEngineName::GraphicsEngine).value()
        )
    );
}

void Ceil::injectDependency() noexcept {
    // duplicated
    auto self = weak_from_this();

    auto mc=Component::make<MoveComponent>(self);
    auto bc = Component::make<BoxComponent>(self);

    bc->UniqueObservable<ColorRect>::subscribe(
        std::dynamic_pointer_cast<GraphicsEngine>(
            owner.lock()->query(SubEngineName::GraphicsEngine).value()
        )
    );
    // end

    mc->attr().set({
        {1024.0f/2, 15.0f/2},
        {1024.0f, 15.0f}
    });
    bc->setTexture({});
}

void Floor::injectDependency() noexcept {
    // duplicated
    auto self = weak_from_this();

    auto mc=Component::make<MoveComponent>(self);
    auto bc = Component::make<BoxComponent>(self);

    bc->UniqueObservable<ColorRect>::subscribe(
        std::dynamic_pointer_cast<GraphicsEngine>(
            owner.lock()->query(SubEngineName::GraphicsEngine).value()
        )
    );
    // end

    mc->attr().set({
        {1024.0f/2, 768.0f - 15.0f/2},
        {1024.0f, 15.0f}
    });
    bc->setTexture({});
}

void RightWall::injectDependency() noexcept {
    // duplicated
    auto self = weak_from_this();

    auto mc=Component::make<MoveComponent>(self);
    auto bc = Component::make<BoxComponent>(self);

    bc->UniqueObservable<ColorRect>::subscribe(
        std::dynamic_pointer_cast<GraphicsEngine>(
            owner.lock()->query(SubEngineName::GraphicsEngine).value()
        )
    );
    // end

    mc->attr().set({
        {1024.0f - 15.0f/2, 768.0f/2},
        {15.0f, 768.0f}
    });
    bc->setTexture({});
}
