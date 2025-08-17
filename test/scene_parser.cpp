#include <gtest/gtest.h>
#include "engine/asset/scene_parser.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Asset;

TEST(SceneParser, ParseSimpleTransform){
    std::string tomlText = R"(
        [[entities]]
        name = "Box"
        [entities.transform]
        position = [1, 2, 3]
        rotation = [0, 0, 0, 1]
        scale = [1, 1, 1]
    )";
    Vec3 position{ .x=1, .y=2, .z=3 };
    auto rotation = unitQuat();
    auto scale = ones();
    TempScene temp = parseSceneFromString(tomlText);

    auto registry = makeDefaultBinderRegistry();
    SceneDescriptor scene = buildScene(temp, registry);

    ASSERT_EQ(scene.entities.size(), 1);
    EXPECT_EQ(scene.entities[0].name, "Box");
    EXPECT_NE(scene.entities[0].transformIndex, INVALID);
    EXPECT_EQ(scene.transforms[0].position, position);
    EXPECT_EQ(scene.transforms[0].rotation, rotation);
    EXPECT_EQ(scene.transforms[0].scale, scale);
}

TEST(SceneParser, ParseEntityWithoutTransform){
    std::string tomlText = R"(
        [[entities]]
        name = "Light"
    )";
    TempScene temp = parseSceneFromString(tomlText);
    auto registry = makeDefaultBinderRegistry();
    SceneDescriptor scene = buildScene(temp, registry);

    ASSERT_EQ(scene.entities.size(), 1);
    EXPECT_EQ(scene.entities[0].name, "Light");
    EXPECT_EQ(scene.entities[0].transformIndex, INVALID);
}

TEST(SceneParser, ParseMultipleEntities){
    std::string tomlText = R"(
        [[entities]]
        name = "Box"
        [entities.transform]
        position = [10, 20, 30]
        rotation = [0, 0, 0, 1]
        scale = [2, 2, 2]

        [[entities]]
        name = "Lamp"
    )";
    TempScene temp = parseSceneFromString(tomlText);
    auto registry = makeDefaultBinderRegistry();
    SceneDescriptor scene = buildScene(temp, registry);

    ASSERT_EQ(scene.entities.size(), 2);
    EXPECT_EQ(scene.entities[0].name, "Box");
    EXPECT_EQ(scene.entities[1].name, "Lamp");
    // First entity has a valid transform
    EXPECT_NE(scene.entities[0].transformIndex, INVALID);
    // Second entity does not have a transform
    EXPECT_EQ(scene.entities[1].transformIndex, INVALID);
    // Check transform values for the first entity
    ASSERT_FALSE(scene.transforms.empty());
    auto& tr = scene.transforms[scene.entities[0].transformIndex];
    EXPECT_EQ(tr.position, (Vec3{.x=10, .y=20, .z=30}));
    EXPECT_EQ(tr.rotation, unitQuat());
    EXPECT_EQ(tr.scale, (Vec3{.x=2, .y=2, .z=2}));
}

TEST(SceneParser, ThrowsOnInvalidVecLength){
    std::string tomlText = R"(
        [[entities]]
        name = "BadBox"
        [entities.transform]
        position = [1, 2]  # invalid length
    )";

    TempScene temp = parseSceneFromString(tomlText);
    auto registry = makeDefaultBinderRegistry();

    EXPECT_THROW({
        auto scene = buildScene(temp, registry);
        (void)scene;
    }, std::runtime_error);
}