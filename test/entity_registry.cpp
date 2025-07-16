#include <gtest/gtest.h>
#include "game/entity_registry.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

TEST(ArchetypeView, SimpleQuery){
    EntityRegistry registry;
    Vec4 testColor{{0.1, 0.2, 0.3, 0.5}};
    
    constexpr auto size = sizeof(TransformComponent) + sizeof(ColorComponent);
    constexpr auto bit = bits_of<TransformComponent, ColorComponent>();
    registry.createEntity(bit,
        dangled<TransformComponent>(identity()),
        ColorComponent{.color = testColor}
    );

    for(auto [tc, cc]: registry.query<TransformComponent, ColorComponent>()){
        EXPECT_EQ(tc.value.position,    zeros());
        EXPECT_EQ(tc.value.rotation, unitQuat());
        EXPECT_EQ(   tc.value.scale,     ones());

        EXPECT_EQ(cc.color, testColor);
    }
}