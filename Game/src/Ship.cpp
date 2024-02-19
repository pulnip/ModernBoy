#include <cassert>
#include <SDL2/SDL_keyboard.h>

#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "ResourceManagerWithSDL.hpp"
#include "Ship.hpp"
#include "Component/Controllable.hpp"
#include "Component/Movable.hpp"
#include "AnimSprite.hpp"

using namespace Game;
using namespace My::Math;

void Ship::updateActor(const Time& deltaTime) noexcept{
    static auto& mc_attr_v=std::static_pointer_cast<Component::Movable>(
        find(Component::Type::Movable)
    )->attr->velocity;

    mc_attr_v={{0.0f, 0.0f}, 0.0f};
}

void Ship::postConstruct() noexcept {
    Interface::postConstruct();

    auto self = weak_from_this();

    auto mc = Component::Interface::make<Component::Movable>(self);

    mc->attr->position.linear={500.0f, 500.0f};
    mc->attr->volume.base={64.0f, 29.0f};

    auto ic = Component::Interface::make<Component::Controllable>(self);

    ic->setKey(SDL_SCANCODE_Q, [&rv = mc->attr->velocity.rotation](){
        rv.get() += -Number::PI;
    });
    ic->setKey(SDL_SCANCODE_E, [&rv = mc->attr->velocity.rotation](){
        rv.get() += Number::PI;
    });
    ic->setKey(SDL_SCANCODE_D,
        [&v=mc->attr->velocity.linear, &r=mc->attr->position.rotation](){
            v += forward(r) * 300.0;
        }
    );
    ic->setKey(SDL_SCANCODE_A,
        [&v=mc->attr->velocity.linear, &r=mc->attr->position.rotation](){
            v += forward(r) * -300.0;
        }
    );

    assert(!owner.expired());
    auto query=owner.lock()->query(SubEngine::Type::ResourceManager);

    assert(query.has_value());
    auto rm=std::dynamic_pointer_cast<
        WithSDL::SubEngine::ResourceManager
    >(query.value());

    assert(rm!=nullptr);

    auto img1=rm->getTexture("Ship01.png");
    auto img2=rm->getTexture("Ship02.png");
    auto img3=rm->getTexture("Ship03.png");
    auto img4=rm->getTexture("Ship04.png");

    auto asc = Component::Interface::make<
        WithSDL::Component::AnimSprite
    >(self);

    std::dynamic_pointer_cast<SubEngine::GraphicsEngine>(
        owner.lock()->query(SubEngine::Type::GraphicsEngine).value()
    )->append(asc);

    asc->setAnimTextures(std::vector<SDL_Texture*>{
        img1.value(), img2.value(), img3.value(), img4.value()
    });
}
