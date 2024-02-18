#include <SDL2/SDL_keyboard.h>

#include "Skin.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "SubEngine/ActorManager.hpp"
#include "Paddle.hpp"
#include "Component/ColoredBody.hpp"
#include "Component/Controllable.hpp"
#include "Component/Movable.hpp"

void Paddle::updateActor(const float &deltaTime) noexcept{
    Actor::Interface* self=this;
    static auto& mc_attr_v_y=std::static_pointer_cast<Movable>(
        self->find(Type::Movable)
    )->attr().velocity.linear.y;
    
    mc_attr_v_y=0.0f;
}

void Paddle::injectDependency() noexcept {
    auto self = weak_from_this();

    // cc = Component::make<Rigidbody>(self);
    auto mc = Component::make<Movable>(self);

    auto bc = Component::make<ColoredBody>(self);
    auto ic = Component::make<Controllable>(self);

    mc->attr().set({
        {15.0f, 384.0f}, {15.0f, 120.0f}
    });

    bc->setTexture({255, 255, 255, 100});
    bc->UniqueObservable<ColorRect>::subscribe(
        std::dynamic_pointer_cast<GraphicsEngine>(
            owner.lock()->query(SubEngine::Type::GraphicsEngine).value()
        )
    );

    ic->setKey(SDL_SCANCODE_S, [&v_y = mc->attr().velocity.linear.y]() {
        v_y += 300.0f;
    });
    ic->setKey(SDL_SCANCODE_W, [&v_y = mc->attr().velocity.linear.y]() {
        v_y += -300.0f;
    });
}
