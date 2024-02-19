#include <cassert>
#include <vector>

#include "Skin.hpp"
#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GameLogic.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "SubEngine/PhysicsSimulator.hpp"
#include "ResourceManagerWithSDL.hpp"
#include "Ball.hpp"
#include "Component/Movable.hpp"
#include "AnimSprite.hpp"

using namespace Game;

void Ball::updateActor(const Time& deltaTime) noexcept{
    static auto& target=*std::static_pointer_cast<Component::Movable>(
        static_cast<Actor::Interface*>(this)->find(Component::Type::Movable)
    )->attr;

    if(target.position.linear.x < -target.volume.size().x){
        UniqueObservable<Status>::notify(Status::GAME_OVER);
    }
}

void Ball::postConstruct() noexcept {
    Interface::postConstruct();

    auto self = weak_from_this();

    auto mc = Component::Interface::make<Component::Movable>(self);

    mc->attr->set({
        {1024.0f/2, 768.0f/2}, {45.0f, 40.0f}
    });
    mc->attr->velocity.linear={-200.0f, 235.0f};

    assert(!owner.expired());
    {
        auto query=owner.lock()->query(SubEngine::Type::ResourceManager);

        assert(query.has_value());
        auto rm=std::dynamic_pointer_cast<
            WithSDL::SubEngine::ResourceManager
        >(query.value());

        auto img1=rm->getTexture("pigeon_1.png");
        assert(img1.has_value());
        auto img2=rm->getTexture("pigeon_2.png");
        assert(img1.has_value());
        auto img3=rm->getTexture("pigeon_3.png");
        assert(img1.has_value());

        auto asc = Component::Interface::make<
            WithSDL::Component::AnimSprite
        >(self);

        std::dynamic_pointer_cast<SubEngine::GraphicsEngine>(
            owner.lock()->query(SubEngine::Type::GraphicsEngine).value()
        )->append(asc);

        asc->setAnimTextures(std::vector<SDL_Texture*>{
            img1.value(), img2.value(), img3.value(), img2.value()
        });
    }

    {
        auto query=owner.lock()->query(SubEngine::Type::GameLogic);

        assert(query.has_value());
        auto gl=std::static_pointer_cast<
            Game::SubEngine::GameLogic
        >(query.value());

        UniqueObservable<Status>::subscribe(gl);
    }

    {
        auto query=owner.lock()->query(SubEngine::Type::PhysicsSimulator);

        // assert(query.has_value());
        // auto ps=std::static_pointer_cast<PhysicsSimulator>(query.value());
    }
}