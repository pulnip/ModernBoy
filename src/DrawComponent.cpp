#include <cassert>
#include <memory>

#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "Actor/Actor.hpp"
#include "Component/DrawComponent.hpp"
#include "Component/MoveComponent.hpp"

void DrawComponent::injectDependency() noexcept{
    drawOrder=initDrawOrder();

    assert(!owner.expired());
    std::shared_ptr<IActor> actor=owner.lock();

    {
        auto mc=actor->find(ComponentName::MoveComponent);

        assert(mc!=nullptr);
        target=std::dynamic_pointer_cast<MoveComponent>(mc);
    }

    {
        auto query=actor->query(SubEngineName::GraphicsEngine);

        assert(query.has_value());
        graphicsEngine=std::dynamic_pointer_cast<GraphicsEngine>(
            query.value()
        );

        assert(!graphicsEngine.expired());
        auto self=std::static_pointer_cast<DrawComponent>(shared_from_this());
        graphicsEngine.lock()->append(self);
    }

    setAttribute();
}
