#include <cassert>
#include <tuple>
#include <vector>
#include "Engine/Core.hpp"
#include "Engine/ActorManager.hpp"
#include "Engine/GameLogic.hpp"
#include "Engine/Logger.hpp"
#include "Engine/PhysicsSimulator.hpp"
#include "Component/Movable.hpp"
#include "WithSDL/Graphics.hpp"
#include "WithSDL/ResourceManager.hpp"
#include "WithSDL/AnimSprite.hpp"
#include "Test/Ball.hpp"

using namespace WithSDL;
using namespace Test;
using namespace My::Math;

void Ball::updateActor(const Game::Time& deltaTime) noexcept{
    using namespace Component;

    auto movable=get(Type::Movable).transform([](auto ability){
        return std::static_pointer_cast<Movable>(ability);
    });
    
    if(movable.has_value()){
        auto value=movable.value();
        const auto& position=value->get().position.linear;
        const auto size=value->get().volume.size();

        if(position.x < -size.x){
            Engine::GameLogic::get()->set(Game::Status::GAME_OVER);
        }
    }
}

void Ball::initAbility() noexcept {
    using namespace Component;

    auto movable = std::make_shared<Movable>(weak_from_this());
    add(movable);

    movable->get().set({
        {1024.0f/2, 768.0f/2}, {45.0f, 40.0f}
    });
    movable->get().velocity.linear={-200.0, 235.0};

    auto textures=std::dynamic_pointer_cast<ResourceManager>(
        ResourceManager::get()
    )->getTexture(
        std::vector<std::string>{
            "pigeon_1.png",
            "pigeon_2.png",
            "pigeon_3.png",
            "pigeon_2.png"
        }
    );
    
    if(textures.has_value()){
        auto animSprite=std::make_shared<AnimSprite>(weak_from_this());
        add(animSprite);
        animSprite->setAnimTextures(textures.value());

        Graphics::get()->append(animSprite);
    }
}