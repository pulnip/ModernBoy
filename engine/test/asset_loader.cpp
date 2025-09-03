#include <gtest/gtest.h>
#include "resource_manager.hpp"
#include "asset/asset_loader.hpp"
#if defined(USE_METAL)
#include "render/metal/material.hpp"
#include "render/metal/submesh.hpp"
#include "render/metal/shader.hpp"
#endif

using namespace ModernBoy;
using namespace ModernBoy::Asset;

MeshTable meshTable;
MaterialSetTable materialSetTable;
SubmeshManager submeshManager;
MaterialManager materialManager;
ShaderManager shaderManager;

TEST(AssetLoader, LoadsResourceOnce){
    std::string tomlText = R"(
    [[entities]]
    name = "Box1"
    [entities.mesh]
    id = "embedded:cube"
        [entities.mesh.shader]
        module = "file:asset/shader/ModernBoy.metallib"
        vsFunc = "vertex_main"
        fsFunc = "fragment_main"
    [[entities]]
    name = "Box2"
    [entities.mesh]
    id = "embedded:cube"
        [entities.mesh.shader]
        module = "file:asset/shader/ModernBoy.metallib"
        vsFunc = "vertex_main"
        fsFunc = "fragment_main"
    )";
    auto temp = Asset::parseSceneFromString(tomlText);
    auto registry = Asset::makeDefaultBinderRegistry();
    auto scene = Asset::buildScene(temp, registry);

    Asset::AssetLoader loader(
        meshTable, materialSetTable,
        submeshManager, materialManager,
        shaderManager, nullptr);

    loader.load(scene);
    const auto& table = loader.get_table();

    // 1 for mesh, 1 for shader
    ASSERT_EQ(Asset::countLoadedResources(table), 2);
}

TEST(AssetLoader, AssignsUniqueIDs) {
    std::string tomlText = R"(
    [[entities]]
    name = "Box1"
    [entities.mesh]
    id = "embedded:cube"
        [entities.mesh.shader]
        module = "file:asset/shader/ModernBoy.metallib"
        vsFunc = "vertex_main"
        fsFunc = "fragment_main"
    [[entities]]
    name = "Box2"
    [entities.mesh]
    id = "embedded:cube"
        [entities.mesh.shader]
        module = "file:asset/shader/ModernBoy.metallib"
        vsFunc = "vertex_main"
        fsFunc = "fragment_main"
    )";
    auto temp = Asset::parseSceneFromString(tomlText);
    auto registry = Asset::makeDefaultBinderRegistry();
    auto scene = Asset::buildScene(temp, registry);

    Asset::AssetLoader loader(
        meshTable, materialSetTable,
        submeshManager, materialManager,
        shaderManager, nullptr);

    loader.load(scene);
    const auto& table = loader.get_table();

    auto idA = table.at("file:asset/shader/ModernBoy.metallib:vertex_main,fragment_main");
    auto idB = table.at("embedded:cube");
    ASSERT_NE(idA, idB);
}
