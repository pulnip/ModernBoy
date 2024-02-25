#include "Engine/Logger.hpp"
#include "Component/Drawable.hpp"
#include "Component/Movable.hpp"
#include "WithSDL/Graphics.hpp"
#include "Test/Wall.hpp"

using namespace My::Math;
using namespace WithSDL;
using namespace Test;

Wall::Wall() noexcept:
    logger(std::make_unique<Engine::BindedLogger>("Actor", "Wall"))
{
    logger->debug("constructed");
}

Wall::~Wall(){
    logger->debug("destructed");
}

void Ceil::initAbility() noexcept {
    // duplicated
    using namespace Component;
    auto self = weak_from_this();

    auto movable=std::make_shared<Movable>(self);
    add(movable);
    movable->get().set({
        {1024.0f/2, 15.0f/2},
        {1024.0f, 15.0f}
    });

    auto colored=std::make_shared<Colored>(self);
    add(colored);
    Graphics::get()->append(colored);
}

void Floor::initAbility() noexcept{
    // duplicated
    using namespace Component;
    auto self = weak_from_this();

    auto movable=std::make_shared<Movable>(self);
    add(movable);
    movable->get().set({
        {1024.0f/2, 768.0f - 15.0f/2},
        {1024.0f, 15.0f}
    });

    auto colored=std::make_shared<Colored>(self);
    add(colored);
    Graphics::get()->append(colored);
}

void RightWall::initAbility() noexcept{
    // duplicated
        using namespace Component;
    auto self = weak_from_this();

    auto movable=std::make_shared<Movable>(self);
    add(movable);
    movable->get().set({
        {1024.0f - 15.0f/2, 768.0f/2},
        {15.0f, 768.0f}
    });

    auto colored=std::make_shared<Colored>(self);
    add(colored);
    Graphics::get()->append(colored);
}
