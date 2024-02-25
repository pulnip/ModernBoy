#include "Math.hpp"
#include "Engine/Logger.hpp"
#include "Component/Movable.hpp"
#include "WithSDL/Graphics.hpp"
#include "WithSDL/ResourceManager.hpp"
#include "WithSDL/AnimSprite.hpp"
#include "Test/Asteroid.hpp"

using namespace WithSDL;
using namespace Test;
using namespace My::Math;

Asteroid::Asteroid() noexcept:
    logger(std::make_unique<Engine::BindedLogger>("Actor", "Asteroid"))
{
    logger->debug("constructed");
}

Asteroid::~Asteroid(){
    logger->debug("destructed");
}

void Asteroid::initAbility() noexcept {
    using namespace Component;
    auto self = weak_from_this();

    auto movable = std::make_shared<Movable>(self);
    add(movable);
    movable->get().position.linear={
        random(0, 1024), random(0, 768)
    };
    movable->get().position.rotation=random(0, 1024) / Number::PI;
    movable->get().velocity.linear=forward(
        random(-Number::PI, Number::PI)
    ) * random(0, 300);
    movable->get().velocity.rotation=random(-Number::PI/2, Number::PI/2);
    movable->get().volume.base={64.0f, 64.0f};

    auto texture=std::dynamic_pointer_cast<ResourceManager>(
        ResourceManager::get()
    )->getTexture("Asteroid.png");
    
    if(texture.has_value()){
        auto sprite=std::make_shared<Sprite>(self, 300);
        add(sprite);
        sprite->setTexture(texture.value());

        Graphics::get()->append(sprite);
    }
}