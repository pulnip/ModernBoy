#include "Skin.hpp"
#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "Wall.hpp"
#include "Component/ColoredBody.hpp"
#include "Component/Movable.hpp"

using namespace Game;

void Wall::postConstruct() noexcept{
    Interface::postConstruct();

    auto self = weak_from_this();

    auto mc = Component::Interface::make<Component::Movable>(self);
    auto bc = Component::Interface::make<Component::ColoredBody>(self);

    std::dynamic_pointer_cast<SubEngine::GraphicsEngine>(
        owner.lock()->query(SubEngine::Type::GraphicsEngine).value()
    )->append(bc);
}

void Ceil::postConstruct() noexcept {
    // duplicated
    Interface::postConstruct();

    auto self = weak_from_this();

    auto mc = Component::Interface::make<Component::Movable>(self);
    auto bc = Component::Interface::make<Component::ColoredBody>(self);

    std::dynamic_pointer_cast<SubEngine::GraphicsEngine>(
        owner.lock()->query(SubEngine::Type::GraphicsEngine).value()
    )->append(bc);
    // end

    mc->attr->set({
        {1024.0f/2, 15.0f/2},
        {1024.0f, 15.0f}
    });
    bc->setTexture({});
}

void Floor::postConstruct() noexcept {
    // duplicated
    Interface::postConstruct();

    auto self = weak_from_this();

    auto mc = Component::Interface::make<Component::Movable>(self);
    auto bc = Component::Interface::make<Component::ColoredBody>(self);

    std::dynamic_pointer_cast<SubEngine::GraphicsEngine>(
        owner.lock()->query(SubEngine::Type::GraphicsEngine).value()
    )->append(bc);
    // end

    mc->attr->set({
        {1024.0f/2, 768.0f - 15.0f/2},
        {1024.0f, 15.0f}
    });
    bc->setTexture({});
}

void RightWall::postConstruct() noexcept {
    // duplicated
    Interface::postConstruct();

    auto self = weak_from_this();

    auto mc = Component::Interface::make<Component::Movable>(self);
    auto bc = Component::Interface::make<Component::ColoredBody>(self);

    std::dynamic_pointer_cast<SubEngine::GraphicsEngine>(
        owner.lock()->query(SubEngine::Type::GraphicsEngine).value()
    )->append(bc);
    // end

    mc->attr->set({
        {1024.0f - 15.0f/2, 768.0f/2},
        {15.0f, 768.0f}
    });
    bc->setTexture({});
}
