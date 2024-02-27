#include <SDL2/SDL_keyboard.h>
#include "Engine/Logger.hpp"
#include "Component/Controllable.hpp"
#include "Component/Movable.hpp"
#include "Component/Drawable.hpp"
#include "WithSDL/Graphics.hpp"
#include "WithSDL/ResourceManager.hpp"
#include "Test/Paddle.hpp"

using namespace My::Math;
using namespace WithSDL;
using namespace Test;

void Paddle::updateActor(const Game::Time& deltaTime) noexcept{
    using namespace Component;
    auto movable=get(Type::Movable).transform([](auto ability){
        return std::static_pointer_cast<Movable>(ability);
    });
    
    if(movable.has_value()){
        movable.value()->get().velocity.linear.y=0;
    }
}

void Paddle::initAbility() noexcept {
    using namespace Component;
    auto self = weak_from_this();

    // cc = std::make_shared<Rigidbody>(self);
    auto movable=std::make_shared<Movable>(self);
    add(movable);
    movable->get().set({
        {15.0f, 384.0f}, {15.0f, 120.0f}
    });

    auto colored=std::make_shared<Colored>(self);
    add(colored);
    Graphics::get()->append(colored);
    colored->setColor(::Skin::TrueColor{255, 255, 255, 100});

    auto controllable=std::make_shared<Controllable>(self);
    add(controllable);
    // warning...?
    controllable->setKey(SDL_SCANCODE_W,
        [movable](){
            movable->get().velocity.linear.y -= 300.0f;
        }
    );
    controllable->setKey(SDL_SCANCODE_S,
        [movable](){
            movable->get().velocity.linear.y += 300.0f;
        }
    );
}
