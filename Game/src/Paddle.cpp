#include <SDL2/SDL_keyboard.h>

#include "SubEngine/GraphicsEngine.hpp"
#include "SubEngine/ActorManager.hpp"
#include "Paddle.hpp"
#include "Component/BoxComponent.hpp"
#include "Component/CollisionComponent.hpp"
#include "Component/InputComponent.hpp"
#include "Component/MoveComponent.hpp"


void Paddle::updateActor(const float &deltaTime) noexcept {
    static auto& mc_attr_v_y=std::static_pointer_cast<MoveComponent>(
        find(ComponentName::MoveComponent)
    )->attr.velocity.linear.y;
    
    mc_attr_v_y=0.0f;
}

void Paddle::injectDependency() noexcept {
    auto self = weak_from_this();

    // cc = IComponent::make<CollisionComponent>(self);
    auto mc = IComponent::make<MoveComponent>(self);

    auto bc = IComponent::make<BoxComponent>(self);
    auto ic = IComponent::make<InputComponent>(self);

    mc->attr.set({
        {15.0f, 384.0f}, {15.0f, 120.0f}
    });

    bc->setTexture({255, 255, 255, 100});
    bc->Observable<ColorRect>::subscribe(
        std::dynamic_pointer_cast<GraphicsEngine>(
            owner.lock()->query(SubEngineName::GraphicsEngine).value()
        )
    );

    ic->setKey(SDL_SCANCODE_S, [&v_y = mc->attr.velocity.linear.y]() {
        v_y += 300.0f;
    });
    ic->setKey(SDL_SCANCODE_W, [&v_y = mc->attr.velocity.linear.y]() {
        v_y += -300.0f;
    });
}
