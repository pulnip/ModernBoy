#include <gtest/gtest.h>
#include "engine/resource_manager.hpp"
#include "engine/asset/asset_loader.hpp"
#if defined(USE_METAL)
#include "engine/render/metal/material.hpp"
#include "engine/render/metal/mesh.hpp"
#include "engine/render/metal/texture.hpp"
#include "engine/render/metal/shader.hpp"
#endif

using namespace ModernBoy;
using namespace ModernBoy::Asset;

const MeshDescriptor desc1{
    .id = "test:mesh1.mbmesh",
    .material_override = {},
    .shader = ShaderDescriptor{}
};
const MeshDescriptor desc2{
    .id = "test:mesh2.mbmesh",
    .material_override = {},
    .shader = ShaderDescriptor{}
};
const MeshDescriptor desc3{
    .id = "embedTest:cube",
    .material_override = {},
    .shader = ShaderDescriptor{}
};

MeshTable meshTable;
MaterialSetTable materialSetTable;
SubmeshManager submeshManager;
MaterialManager materialManager;
ShaderManager shaderManager;

TEST(AssetLoader, LoadsFileOnce){
    Asset::AssetLoader loader(
        meshTable, materialSetTable,
        submeshManager, materialManager,
        shaderManager, nullptr);

    SceneDescriptor scene;
    scene.meshes.push_back(desc1);
    scene.meshes.push_back(desc1);

    loader.load(scene);
    const auto& table = loader.get_table();

    ASSERT_EQ(table.size(), 1);
}

TEST(AssetLoader, LoadsEmbedded){
    Asset::AssetLoader loader(
        meshTable, materialSetTable,
        submeshManager, materialManager,
        shaderManager, nullptr);

    SceneDescriptor scene;

    scene.meshes.push_back(desc3);

    loader.load(scene);
    const auto& table = loader.get_table();

    ASSERT_EQ(table.size(), 1);
    auto it = table.find("embedTest:cube");
    ASSERT_NE(it, table.end());
}

TEST(AssetLoader, AssignsUniqueIDs) {
    Asset::AssetLoader loader(
        meshTable, materialSetTable,
        submeshManager, materialManager,
        shaderManager, nullptr);

    SceneDescriptor scene;
    scene.meshes.push_back(desc1);
    scene.meshes.push_back(desc2);

    loader.load(scene);
    const auto& table = loader.get_table();

    auto idA = table.at("test:mesh1.mbmesh");
    auto idB = table.at("test:mesh2.mbmesh");
    ASSERT_NE(idA, idB);
}
