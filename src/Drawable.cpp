#include <cassert>
#include <memory>

#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "Actor/Actor.hpp"
#include "Component/Drawable.hpp"
#include "Component/Movable.hpp"

using namespace Game;
using namespace Game::Component;

void Drawable::postConstruct() noexcept{
    assert(!owner.expired());
    auto query=owner.lock()->find(Type::Movable);
    if(query != nullptr){
        target=std::dynamic_pointer_cast<Movable>(query);
    }

    setCanvas();

    if(not canvas.expired()){
        canvas.lock()->append(
            std::dynamic_pointer_cast<Drawable>(
                owner.lock()->find(Type::Drawable)
            )
        );
    }
}

void Drawable::setCanvas() noexcept{
    assert(!owner.expired());
    auto query=owner.lock()->query(SubEngine::Type::GraphicsEngine);

    if(query.has_value()){
        canvas=std::dynamic_pointer_cast<
            SubEngine::GraphicsEngine
        >(query.value());
    }
}
