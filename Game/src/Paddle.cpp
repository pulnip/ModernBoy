#include <SDL2/SDL_keyboard.h>

#include "Skin.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "SubEngine/ActorManager.hpp"
#include "Paddle.hpp"
#include "Component/ColoredBody.hpp"
#include "Component/Controllable.hpp"
#include "Component/Movable.hpp"

using namespace Game;

void Paddle::updateActor(const Time& deltaTime) noexcept{
    static auto& mc_attr_v_y=std::static_pointer_cast<Component::Movable>(
        find(Component::Type::Movable)
    )->attr->velocity.linear.y;
    
    mc_attr_v_y=0.0f;
}

void Paddle::postConstruct() noexcept {
    Interface::postConstruct();

    auto self = weak_from_this();

    // cc = Component::Interface::make<Rigidbody>(self);
    auto mc = Component::Interface::make<Component::Movable>(self);

    auto bc = Component::Interface::make<Component::ColoredBody>(self);
    auto ic = Component::Interface::make<Component::Controllable>(self);

    mc->attr->set({
        {15.0f, 384.0f}, {15.0f, 120.0f}
    });

    bc->setTexture(std::make_shared<Skin::TrueColor>(255, 255, 255, 100));
    std::dynamic_pointer_cast<SubEngine::GraphicsEngine>(
        owner.lock()->query(SubEngine::Type::GraphicsEngine).value()
    )->append(bc);

    ic->setKey(SDL_SCANCODE_S, [&v_y = mc->attr->velocity.linear.y]() {
        v_y += 300.0f;
    });
    ic->setKey(SDL_SCANCODE_W, [&v_y = mc->attr->velocity.linear.y]() {
        v_y += -300.0f;
    });
}
