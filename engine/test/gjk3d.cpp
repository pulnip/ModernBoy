#include <gtest/gtest.h>
#include "physics/physics.hpp"
#include <vector>

using namespace ModernBoy;

//— common shapes —//

// AABB unit cube [0,1]^3
static const Vec3 cubeBase[8] = {
    {{0,0,0}}, {{1,0,0}}, {{1,1,0}}, {{0,1,0}},
    {{0,0,1}}, {{1,0,1}}, {{1,1,1}}, {{0,1,1}}
};

// Tetrahedron with three right-triangle faces + one equilateral face
static const Vec3 tetraBase[4] = {
    {{0,0,0}},
    {{1,0,0}},
    {{0,1,0}},
    {{0,0,1}}
};

// Thin box: very small thickness in Z
static const Vec3 thinBox[8] = {
    {{0,0,0}}, {{2,0,0}}, {{2,2,0}}, {{0,2,0}},
    {{0,0,1e-6f}}, {{2,0,1e-6f}}, {{2,2,1e-6f}}, {{0,2,1e-6f}}
};

// Lower‐dimensional: point, line segment, triangle
static const Vec3 pt0[1]    = {{{0,0,0}}};
static const Vec3 pt1[1]    = {{{1,1,1}}};
static const Vec3 line0[2]  = {{{0,0,0}}, {{1,0,0}}};
static const Vec3 line1[2]  = {{{1,1,1}}, {{2,1,1}}};
static const Vec3 tri0[3]   = {{{0,0,0}}, {{1,0,0}}, {{0,1,0}}};

// Regular icosahedron (12 vertices)
static const float φ = 1.61803398875f, invφ = 0.61803398875f;
static const Vec3 icosa[12] = {
    {{ 0,  1,  φ}}, {{ 0, -1,  φ}}, {{ 0,  1, -φ}}, {{ 0, -1, -φ}},
    {{ 1,  φ,  0}}, {{-1,  φ,  0}}, {{ 1, -φ,  0}}, {{-1, -φ,  0}},
    {{ φ,  0,  1}}, {{-φ,  0,  1}}, {{ φ,  0, -1}}, {{-φ,  0, -1}}
};

// Regular dodecahedron (20 vertices)
static const Vec3 dodeca[20] = {
    {{ 1,  1,  1}}, {{ 1,  1, -1}}, {{ 1, -1,  1}}, {{ 1, -1, -1}},
    {{-1,  1,  1}}, {{-1,  1, -1}}, {{-1, -1,  1}}, {{-1, -1, -1}},
    {{ 0,  invφ,  φ}}, {{ 0, -invφ,  φ}}, {{ 0,  invφ, -φ}}, {{ 0, -invφ, -φ}},
    {{ invφ,  φ,  0}}, {{-invφ,  φ,  0}}, {{ invφ, -φ,  0}}, {{-invφ, -φ,  0}},
    {{ φ,  0,  invφ}}, {{ φ,  0, -invφ}}, {{-φ,  0,  invφ}}, {{-φ,  0, -invφ}}
};

//— utility to translate a shape —//
static void translateShape(const Vec3* src, int n, Vec3* dst, const Vec3& off) {
    for(int i=0;i<n;++i) {
        dst[i] = src[i] + off;
    }
}

// 1. Basic handy case
TEST(GJK3D, Basic) {
    Vec3 a[8], b[8];
    translateShape(cubeBase, 8, a, {{0,0,0}});
    // exclude: shift by (3,0,0)
    translateShape(cubeBase, 8, b, {{3,0,0}});
    EXPECT_FALSE(gjk(a,8,b,8));
    EXPECT_FALSE(gjk(b,8,a,8));
    // intersect: shift by (0.5,0.5,0.5)
    translateShape(cubeBase, 8, b, {{0.5f,0.5f,0.5f}});
    EXPECT_TRUE(gjk(a,8,b,8));
    EXPECT_TRUE(gjk(b,8,a,8));
}

// 2. tetrahedron-vs-tetrahedron
TEST(GJK3D, Tetrahedron) {
    Vec3 a[4], b[4];
    translateShape(tetraBase,4,a,{{0,0,0}});
    // exclude
    translateShape(tetraBase,4,b,{{2,2,2}});
    EXPECT_FALSE(gjk(a,4,b,4));
    EXPECT_FALSE(gjk(b,4,a,4));
    // intersect
    translateShape(tetraBase,4,b,{{0.2f,0.2f,0.2f}});
    EXPECT_TRUE(gjk(a,4,b,4));
    EXPECT_TRUE(gjk(b,4,a,4));
    // include (same)
    translateShape(tetraBase,4,b,{{0,0,0}});
    EXPECT_TRUE(gjk(a,4,b,4));
    EXPECT_TRUE(gjk(b,4,a,4));
}

// 3. cube-vs-cube
TEST(GJK3D, Box) {
    Vec3 a[8], b[8];
    translateShape(cubeBase,8,a,{{-1,-1,-1}});  // box from -1..0
    // exclude
    translateShape(cubeBase,8,b,{{2,2,2}});
    EXPECT_FALSE(gjk(a,8,b,8));
    EXPECT_FALSE(gjk(b,8,a,8));
    // intersect
    translateShape(cubeBase,8,b,{{-0.5f,-0.5f,-0.5f}});
    EXPECT_TRUE(gjk(a,8,b,8));
    EXPECT_TRUE(gjk(b,8,a,8));
    // include (same)
    translateShape(cubeBase,8,b,{{-1,-1,-1}});
    EXPECT_TRUE(gjk(a,8,b,8));
    EXPECT_TRUE(gjk(b,8,a,8));
}

// 4. tetrahedron vs cube
TEST(GJK3D, TetrahedronBox) {
    Vec3 t[4], b[8];
    translateShape(tetraBase,4,t,{{0,0,0}});
    translateShape(cubeBase,8,b,{{1,1,1}});
    // exclude
    EXPECT_FALSE(gjk(t,4,b,8));
    EXPECT_FALSE(gjk(b,8,t,4));
    translateShape(cubeBase,8,b,{{0.3f,0.3f,0.3f}});
    // intersect
    EXPECT_TRUE(gjk(t,4,b,8));
    EXPECT_TRUE(gjk(b,8,t,4));
    translateShape(cubeBase,8,b,{{-1,-1,-1}});
    // include
    EXPECT_TRUE(gjk(t,4,b,8));
    EXPECT_TRUE(gjk(b,8,t,4));
}

// 5. Lower-dimensional combos (point, line, triangle)
TEST(GJK3D, LowerDimensional) {
    // point-line
    EXPECT_TRUE (gjk(pt0,1, line0,2));
    EXPECT_TRUE (gjk(line0,2, pt0,1));
    EXPECT_FALSE(gjk(pt1,1, line0,2));
    EXPECT_FALSE(gjk(line0,2, pt1,1));
    // point-triangle
    EXPECT_TRUE (gjk(pt0,1, tri0,3));
    EXPECT_TRUE (gjk(tri0,3, pt0,1));
    EXPECT_FALSE(gjk(pt1,1, tri0,3));
    EXPECT_FALSE(gjk(tri0,3, pt1,1));
    // line-triangle
    EXPECT_TRUE (gjk(line0,2, tri0,3));
    EXPECT_TRUE (gjk(tri0,3, line0,2));
    EXPECT_FALSE(gjk(line1,2, tri0,3));
    EXPECT_FALSE(gjk(tri0,3, line1,2));
}

// 6. Touching case (exact boundary contact)
TEST(GJK3D, Touching) {
    Vec3 t1[4], t2[4];
    translateShape(tetraBase,4,t1,{{0,0,0}});
    // shift by face centroid (1/3,1/3,1/3): they share the face
    translateShape(tetraBase,4,t2,{{1.f/3,1.f/3,1.f/3}});
    EXPECT_TRUE(gjk(t1,4,t2,4));
    EXPECT_TRUE(gjk(t2,4,t1,4));
}

// 7. Thin/small volume touch
TEST(GJK3D, ThinOrSmall) {
    EXPECT_TRUE(gjk(thinBox,8,tetraBase,4));
    EXPECT_TRUE(gjk(tetraBase,4,thinBox,8));
}

// 8. Complex shapes: dodecahedron vs icosahedron
TEST(GJK3D, ComplexPolyhedra) {
    // exclude
    {
        std::vector<Vec3> ico(std::begin(icosa), std::end(icosa));
        for(auto& p: ico) p += Vec3{{5,5,5}};
        EXPECT_FALSE(gjk(dodeca,20, ico.data(), (int)ico.size()));
        EXPECT_FALSE(gjk(ico.data(), (int)ico.size(), dodeca,20));
    }
    // intersect
    {
        std::vector<Vec3> ico(std::begin(icosa), std::end(icosa));
        for(auto& p: ico) p += Vec3{{0.5f,0.5f,0.5f}};
        EXPECT_TRUE(gjk(dodeca,20, ico.data(), (int)ico.size()));
        EXPECT_TRUE(gjk(ico.data(), (int)ico.size(), dodeca,20));
    }
    // include: small icosa inside dodeca
    {
        std::vector<Vec3> ico(std::begin(icosa), std::end(icosa));
        for(auto& p: ico) p *= 0.5f;
        EXPECT_TRUE(gjk(dodeca,20, ico.data(), (int)ico.size()));
        EXPECT_TRUE(gjk(ico.data(), (int)ico.size(), dodeca,20));
    }
}