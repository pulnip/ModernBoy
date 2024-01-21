#include <cassert>
#include <SDL2/SDL_keyboard.h>

#include "SubEngine/ActorManager.hpp"
#include "ResourceManagerWithSDL.hpp"
#include "Ship.hpp"
#include "Component/InputComponent.hpp"
#include "Component/MoveComponent.hpp"
#include "AnimSpriteComponent.hpp"

void Ship::updateActor(const float& deltaTime) noexcept {
    static auto& mc_attr_v=std::static_pointer_cast<MoveComponent>(
        find(ComponentName::MoveComponent)
    )->attr.velocity;

    mc_attr_v={{0.0f, 0.0f}, 0.0f};
}

void Ship::injectDependency() noexcept {
    auto self = weak_from_this();

    auto mc = IComponent::make<MoveComponent>(self);

    auto sc = IComponent::make<AnimSpriteComponent>(self);
    auto ic = IComponent::make<InputComponent>(self);

    mc->attr.position.linear={500.0f, 500.0f};

    ic->setKey(SDL_SCANCODE_Q, [&rv = mc->attr.velocity.rotation](){
        rv += -Math::PI;
    });
    ic->setKey(SDL_SCANCODE_E, [&rv = mc->attr.velocity.rotation](){
        rv += Math::PI;
    });
    ic->setKey(SDL_SCANCODE_D,
        [&v = mc->attr.velocity.linear, &r = mc->attr.position.rotation](){
            v += Vector2::forward(r) * 300.0f;
        }
    );
    ic->setKey(SDL_SCANCODE_A,
        [&v = mc->attr.velocity.linear, &r = mc->attr.position.rotation](){
            v += Vector2::forward(r) * -300.0f;
        }
    );

    assert(!owner.expired());
    auto query=owner.lock()->query(SubEngineName::ResourceManager);

    assert(query.has_value());
    auto manager=std::dynamic_pointer_cast<ResourceManagerWithSDL>(query.value());

    assert(manager!=nullptr);

    std::vector<std::optional<SDL_Texture *>> opAnims={
        manager->getTexture("Ship01.png"),
        manager->getTexture("Ship02.png"),
        manager->getTexture("Ship03.png"),
        manager->getTexture("Ship04.png")
    };

    std::vector<SDL_Texture *> anims;
    for (auto &o : opAnims) {
        if (o.has_value()) {
            anims.emplace_back(o.value());
        }
    }

    sc->setAnimTextures(anims);
}
