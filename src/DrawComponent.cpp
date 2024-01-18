#include <cassert>
#include <memory>

#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "Actor/IActor.hpp"
#include "Component/DrawComponent.hpp"
#include "Component/MoveComponent.hpp"

void DrawComponent::injectDependency() noexcept {
    assert(!owner.expired());
    auto actor=owner.lock();
    {
        auto query=actor->queryComponent(ComponentName::MoveComponent);
        
        assert(query.has_value() and !query.value().expired());
        target=std::dynamic_pointer_cast<MoveComponent>(
            query.value().lock()
        );
    }

    auto wpActorManager=actor->getActorManager();
    assert(!wpActorManager.expired());
    auto actorManager=wpActorManager.lock();
    {
        auto query=actorManager->query(SubEngineName::GraphicsEngine);

        assert(query.has_value());
        graphicsEngine=std::dynamic_pointer_cast<GraphicsEngine>(
            query.value()
        );
    }
}
