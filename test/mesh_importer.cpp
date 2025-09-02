#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include "engine/asset/mesh_importer.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Asset;
namespace fs = std::filesystem;

// Helpers
static bool indicesInRange(const CookedSubmesh& sm){
    const auto vc = static_cast<uint32_t>(sm.vertices.size());
    for(uint32_t i : sm.indices){ if(i >= vc) return false; }
    return true;
}

TEST(EmbeddedMesh, TriangleBasic){
    auto emb = Asset::loadEmbeddedMesh("triangle");
    ASSERT_EQ(emb.indices.size(), 3u);
    ASSERT_EQ(emb.vertices.size(), 3u);
}

TEST(EmbeddedMesh, RectangleBasic){
    auto emb = Asset::loadEmbeddedMesh("rectangle");
    ASSERT_EQ(emb.vertices.size(), 4u);
    ASSERT_EQ(emb.indices.size(), 6u);
}

TEST(EmbeddedMesh, CubeBasic){
    auto emb = Asset::loadEmbeddedMesh("cube");
    ASSERT_EQ(emb.vertices.size(), 24u); // 6 faces * 4 verts
    ASSERT_EQ(emb.indices.size(), 36u);  // 12 tris
}

TEST(EmbeddedMesh, SphereBasic){
    auto emb = Asset::loadEmbeddedMesh("sphere");
    ASSERT_GT(emb.vertices.size(), 0u);
    ASSERT_GT(emb.indices.size(), 0u);
}

TEST(ImportCooked, ImportIfAssetExists){
    // Try to import a tiny asset if present. If not present, skip gracefully.
    // You can drop a small OBJ at tests/assets/tri.obj for this test to fully run.
    const fs::path candidate = fs::path("tests/assets/tri.obj");
    if(!fs::exists(candidate)){
        GTEST_SKIP() << "No test asset found at " << candidate.string() << ", skipping import test.";
    }

    CookOptions opt{};
    opt.axes = AxisInfo{ AxisInfo::RH, 'Y', 'Z', true, 1.0f };

    CookedMesh mesh = Asset::importMeshFiles(std::vector{candidate}, opt);

    // Basic structure checks
    ASSERT_GT(mesh.submeshes.size(), 0u);

    // Axis/AABB sanity
    EXPECT_LE(mesh.aabb.min.x, mesh.aabb.max.x);
    EXPECT_LE(mesh.aabb.min.y, mesh.aabb.max.y);
    EXPECT_LE(mesh.aabb.min.z, mesh.aabb.max.z);

    // Each submesh must be draw-ready
    for(const auto& sm : mesh.submeshes){
        EXPECT_GT(sm.vertices.size(), 0u);
        EXPECT_GT(sm.indices.size(), 0u);
        EXPECT_TRUE(indicesInRange(sm));
        // We triangulate in importer
        EXPECT_EQ(sm.primitiveType, PrimitiveType::TriangleList);
    }
}

TEST(ImportCooked, MaterialsAreURIBased){
    // This test is structural: we only verify that textures store URIs and flags, not pixels.
    // If you have an asset with materials, point to it here; otherwise the test passes vacuously.
    const fs::path candidate = fs::path("tests/assets/material.obj");
    if(!fs::exists(candidate)){
        GTEST_SKIP() << "No material test asset found at " << candidate.string() << ", skipping.";
    }

    CookedMesh mesh = Asset::importMeshFiles(std::vector{candidate}, {});

    // If any materials exist, verify texture entries are URI-only
    if(!mesh.materials.empty()){
        for(const auto& [name, mat] : mesh.materials){
            for(const auto& [slot, tex] : mat.textures){
                (void)slot; // unused in assert macros
                EXPECT_FALSE(tex.uri.empty());
                // flags bit0 is sRGB for BaseColor/Emissive, may be 0 for others
                if(tex.usage == TextureUsage::BaseColor || tex.usage == TextureUsage::Emissive){
                    EXPECT_NE((tex.flags & TextureFlag_SRGB), 0);
                }
            }
        }
    }
}