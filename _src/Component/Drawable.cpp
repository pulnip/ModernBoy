#include <cassert>
#include "Engine/SystemLocator.hpp"
#include "Actor/Actor.hpp"
#include "Component/Traits.hpp"
#include "Component/Drawable/Drawable.hpp"
#include "Component/Movable/Movable.hpp"

Drawable::~Drawable(){
    Getter::graphics().remove(id);
}

void Drawable::update(const Time&, Actor& actor) noexcept{
    constexpr auto MOVABLE = ComponentTraits::Type::Movable;

    auto comp=actor.get(MOVABLE);
    if(comp.has_value()){
        if(auto movable = std::dynamic_pointer_cast<Movable>(comp.value())){
            drawAt(movable->get().position);
        } else assert(false);
    } else{
        drawAt({});
    }
}