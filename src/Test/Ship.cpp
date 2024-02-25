#include <SDL2/SDL_keyboard.h>
#include "Engine/Logger.hpp"
#include "Component/Controllable.hpp"
#include "Component/Movable.hpp"
#include "WithSDL/Graphics.hpp"
#include "WithSDL/ResourceManager.hpp"
#include "WithSDL/AnimSprite.hpp"
#include "Test/Ship.hpp"

using namespace My::Math;
using namespace WithSDL;
using namespace Test;

Ship::Ship() noexcept:
    logger(std::make_unique<Engine::BindedLogger>("Actor", "Ship"))
{
    logger->debug("constructed");
}

Ship::~Ship(){
    logger->debug("destructed");
}

void Ship::updateActor(const Game::Time& deltaTime) noexcept{
    using namespace Component;
    auto movable=get(Type::Movable).transform([](auto ability){
        return std::static_pointer_cast<Movable>(ability);
    });
    
    if(movable.has_value()){
        movable.value()->get().velocity={{0.0, 0.0}, 0.0};
    }
}

void Ship::initAbility() noexcept {
    using namespace Component;
    auto self = weak_from_this();

    auto movable=std::make_shared<Movable>(self);
    add(movable);
    movable->get().set({
        {500.0, 500.0}, {64.0, 29.0}
    });

    auto textures=std::dynamic_pointer_cast<ResourceManager>(
        ResourceManager::get()
    )->getTexture(
        std::vector<std::string>{
            "Ship01.png",
            "Ship02.png",
            "Ship03.png",
            "Ship04.png"
        }
    );
    if(textures.has_value()){
        auto animSprite=std::make_shared<AnimSprite>(self);
        add(animSprite);
        animSprite->setAnimTextures(textures.value());

        Graphics::get()->append(animSprite);
    }

    auto controllable=std::make_shared<Controllable>(self);
    add(controllable);
    // warning...?
    controllable->setKey(SDL_SCANCODE_E,
        [movable](){
            movable->get().velocity.rotation.get() += Number::PI;
        }
    );
    controllable->setKey(SDL_SCANCODE_Q,
        [movable](){
            movable->get().velocity.rotation.get() -= Number::PI;
        }
    );
    controllable->setKey(SDL_SCANCODE_D,
        [movable](){
            auto& v=movable->get().velocity.linear;
            auto& r=movable->get().position.rotation;
            v += forward(r) * 300.0;
        }
    );
    controllable->setKey(SDL_SCANCODE_A,
        [movable](){
            auto& v=movable->get().velocity.linear;
            auto& r=movable->get().position.rotation;
            v -= forward(r) * 300.0;
        }
    );
}
