#include <cassert>
#include <vector>

#include "Skin.hpp"
#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GameLogic.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "SubEngine/PhysicsSimulator.hpp"
#include "ResourceManagerWithSDL.hpp"
#include "Ball.hpp"
#include "Component/MoveComponent.hpp"
#include "AnimSpriteComponent.hpp"

void Ball::updateActor(const float &deltaTime) noexcept{
    static auto& target=std::static_pointer_cast<MoveComponent>(
        static_cast<IActor*>(this)->find(ComponentName::MoveComponent)
    )->attr();

    if(target.position.linear.x < -target.volume.size().x){
        UniqueObservable<GameStatus>::notify(GameStatus::GAME_OVER);
    }
}

void Ball::injectDependency() noexcept {
    auto self = weak_from_this();

    auto mc = Component::make<MoveComponent>(self);

    mc->attr().set({
        {1024.0f/2, 768.0f/2}, {45.0f, 40.0f}
    });
    mc->attr().velocity.linear={-200.0f, 235.0f};

    assert(!owner.expired());
    {
        auto query=owner.lock()->query(SubEngineName::ResourceManager);

        assert(query.has_value());
        auto rm=std::dynamic_pointer_cast<ResourceManagerWithSDL>(query.value());

        auto img1=rm->getTexture("pigeon_1.png");
        assert(img1.has_value());
        auto img2=rm->getTexture("pigeon_2.png");
        assert(img1.has_value());
        auto img3=rm->getTexture("pigeon_3.png");
        assert(img1.has_value());

        auto asc = Component::make<AnimSpriteComponent>(self);

        asc->setAnimTextures(std::vector<SDL_Texture*>{
            img1.value(), img2.value(), img3.value(), img2.value()
        });
    }

    {
        auto query=owner.lock()->query(SubEngineName::GameLogic);

        assert(query.has_value());
        auto gl=std::static_pointer_cast<GameLogic>(query.value());

        UniqueObservable<GameStatus>::subscribe(gl);
    }

    {
        auto query=owner.lock()->query(SubEngineName::PhysicsSimulator);

        // assert(query.has_value());
        // auto ps=std::static_pointer_cast<PhysicsSimulator>(query.value());
    }
}