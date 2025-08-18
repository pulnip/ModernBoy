#include <gtest/gtest.h>
#include "engine/resource_manager.hpp"
#include "engine/asset/asset_loader.hpp"
#if defined(USE_METAL)
#include "engine/render/metal/mesh.hpp"
#endif

using namespace ModernBoy;
using namespace ModernBoy::Asset;

const MeshDescriptor desc1{
    .id = "test:mesh1.mbmesh",
    .material_override = MaterialDescriptor{},
    .shader = ShaderDescriptor{}
};
const MeshDescriptor desc2{
    .id = "test:mesh2.mbmesh",
    .material_override = MaterialDescriptor{},
    .shader = ShaderDescriptor{}
};
const MeshDescriptor desc3{
    .id = "embedTest:cube",
    .material_override = MaterialDescriptor{},
    .shader = ShaderDescriptor{}
};

TEST(AssetLoader, LoadsFileOnce){
    MeshManager meshManager;
    Asset::AssetLoader loader(meshManager, nullptr);

    SceneDescriptor scene;
    scene.meshes.push_back(desc1);
    scene.meshes.push_back(desc1);

    loader.load(scene);
    const auto& table = loader.get_table();

    ASSERT_EQ(table.size(), 1);
}

TEST(AssetLoader, LoadsEmbedded){
    MeshManager meshManager;
    Asset::AssetLoader loader(meshManager, nullptr);

    SceneDescriptor scene;

    scene.meshes.push_back(desc3);

    loader.load(scene);
    const auto& table = loader.get_table();

    ASSERT_EQ(table.size(), 1);
    auto it = table.find("embedTest:cube");
    ASSERT_NE(it, table.end());
}

TEST(AssetLoader, AssignsUniqueIDs) {
    MeshManager meshManager;
    Asset::AssetLoader loader(meshManager, nullptr);

    SceneDescriptor scene;
    scene.meshes.push_back(desc1);
    scene.meshes.push_back(desc2);

    loader.load(scene);
    const auto& table = loader.get_table();

    auto idA = table.at("test:mesh1.mbmesh");
    auto idB = table.at("test:mesh2.mbmesh");
    ASSERT_NE(idA, idB);
}
