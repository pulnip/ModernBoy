#include <cassert>
#include <memory>

#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "Actor/Actor.hpp"
#include "Component/Drawable.hpp"
#include "Component/Movable.hpp"

void Drawable::injectDependency() noexcept{
    drawOrder=initDrawOrder();

    assert(!owner.expired());
    std::shared_ptr<Actor::Interface> actor=owner.lock();

    {
        auto mc=actor->find(Type::Movable);

        assert(mc!=nullptr);
        target=std::dynamic_pointer_cast<Movable>(mc);
    }

    {
        auto query=actor->query(SubEngine::Type::GraphicsEngine);

        assert(query.has_value());
        graphicsEngine=std::dynamic_pointer_cast<GraphicsEngine>(
            query.value()
        );

        assert(!graphicsEngine.expired());
        auto self=std::static_pointer_cast<Drawable>(shared_from_this());
        graphicsEngine.lock()->append(self);
    }

    setAttribute();
}
