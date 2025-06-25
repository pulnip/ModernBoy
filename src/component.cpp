#include <limits>
#include "component.hpp"

using namespace ModernBoy;

bool ModernBoy::subset(ArchetypeBit a, ArchetypeBit b){
    return (a & b) == a;
}

template<>
TransformComponent ModernBoy::dangled(Transform transform){
    return {std::numeric_limits<EntityID>::max(),
        true, transform };
}
template<>
CameraComponent ModernBoy::dangled(Camera camera){
    return {std::numeric_limits<EntityID>::max(),
        true, camera };

}
template<>
InputComponent ModernBoy::dangled(Input::InputMap&& map){
    return {std::numeric_limits<EntityID>::max(),
        true, map };
}