#include <cmath>
#include <gtest/gtest.h>
#include "engine/physics/physics.hpp"

using namespace ModernBoy;

TEST(Overlap, Overlapped){
    static_assert(overlap(0.0f, 2.0f, 1.0f, 3.0f));
}

TEST(Oberlap, NonOverlapped) {
    static_assert(!overlap(0.0f, 1.0f, 2.0f, 3.0f));
}

TEST(Overlap, Boundary) {
    static_assert(overlap(0.0f, 1.0f, 1.0f, 2.0f));
}

static constexpr Vec3 AXIS_X{{2.0f, 0.0f, 0.0f}};
static constexpr Vec3 AXIS_Y{{0.0f, 2.0f, 0.0f}};
static constexpr Vec3 AXIS_Z{{0.0f, 0.0f, 2.0f}};

TEST(OBB, EquivalentOverlapped) {
    constexpr Vec3 c1{{0.0f, 0.0f, 0.0f}}, c2 = c1;
    EXPECT_TRUE(obb(c1, AXIS_X, AXIS_Y, AXIS_Z,
                    c2, AXIS_X, AXIS_Y, AXIS_Z) );
}

TEST(OBB, Overlapped) {
    Vec3 c1{{0.0f, 0.0f, 0.0f}};
    Vec3 c2{{1.0f, 1.0f, 1.0f}};
    EXPECT_TRUE(obb(c1, AXIS_X, AXIS_Y, AXIS_Z,
                    c2, AXIS_X, AXIS_Y, AXIS_Z));
}

TEST(OBB, NonOverlapped) {
    constexpr Vec3 c1{{0.0f, 0.0f, 0.0f}};
    constexpr Vec3 c2{{5.0f, 0.0f, 0.0f}};
    EXPECT_FALSE(obb(c1, AXIS_X, AXIS_Y, AXIS_Z,
                     c2, AXIS_X, AXIS_Y, AXIS_Z));
}

TEST(OBB, Boundary) {
    constexpr Vec3 c1{{0.0f, 0.0f, 0.0f}};
    constexpr Vec3 c2{{2.0f, 0.0f, 0.0f}};
    EXPECT_TRUE(obb(c1, AXIS_X, AXIS_Y, AXIS_Z,
                    c2, AXIS_X, AXIS_Y, AXIS_Z));
}

TEST(OBB, RotatedOverlap) {
    Vec3 c1{{0.0f, 0.0f, 0.0f}};
    Vec3 c2{{0.5f, 0.5f, 0.0f}};

    float s2 = std::sqrt(2.0f);
    Vec3 rX{{  s2,   s2, 0.0f}};
    Vec3 rY{{ -s2,   s2, 0.0f}};
    Vec3 rZ{{0.0f, 0.0f, 2.0f}};

    EXPECT_TRUE(obb(c1, rX, rY, rZ,
                    c2, rX, rY, rZ));
}

TEST(OBB, RotatedNonOverlap) {
    Vec3 c1{{0.0f, 0.0f, 0.0f}};
    Vec3 c2{{3.0f, 3.0f, 0.0f}};

    float s2 = std::sqrt(2.0f);
    Vec3 rX{{  s2,   s2, 0.0f}};
    Vec3 rY{{ -s2,   s2, 0.0f}};
    Vec3 rZ{{0.0f, 0.0f, 2.0f}};

    EXPECT_FALSE(obb(c1, rX, rY, rZ,
                     c2, rX, rY, rZ));
}

TEST(OBB, RotatedBoundaryTouch) {
    Vec3 c1{{         0.0f,          0.0f, 0.0f}};
    Vec3 c2{{1.4142135624f, 1.4142135624f, 0.0f}};

    float s2 = std::sqrt(2.0f);
    Vec3 rX{{  s2,   s2, 0.0f}};
    Vec3 rY{{ -s2,   s2, 0.0f}};
    Vec3 rZ{{0.0f, 0.0f, 2.0f}};

    EXPECT_TRUE(obb(c1, rX, rY, rZ,
                    c2, rX, rY, rZ));
}
