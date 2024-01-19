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
    }
}
