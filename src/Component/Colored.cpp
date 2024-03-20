#include "Component/Drawable/Colored.hpp"
#include "Engine/SystemLocator.hpp"

void Colored::drawAt(const Pos& pos) noexcept{
    Getter::graphics().request(
        id, {updateOrder, {color, pos}}
    );
}
