#include <gtest/gtest.h>
#include "physics/physics.hpp"

using namespace ModernBoy;

Vec2 triangle1[3] = {{{0, 1}}, {{1, -1}}, {{-1, -1}}};
Vec2 triangle2[3] = {{{1, 1}}, {{3,  1}}, {{ 2, -1}}};
Vec2 triangle3[3] = {{{1, 1}}, {{0, -1}}, {{ 2, -1}}};
Vec2 quad2[4]   = {{{0.4, 0}}, {{1, 0}}, {{1, 1}}, {{0.5, 1}}};
Vec2 quad1[4]   = {{{1,  0}}, {{2,  0}}, {{ 2,  2}}, {{ 0, 2}}};
Vec2 square1[4] = {{{1,  1}}, {{1, -1}}, {{-1, -1}}, {{-1, 1}}};
Vec2 square2[4] = {{{0,  0}}, {{2,  0}}, {{ 2,  2}}, {{ 0, 2}}};
Vec2 square3[4] = {{{2, -1}}, {{3, -1}}, {{ 3,  1}}, {{ 2, 1}}};

TEST(GJK, Basic){

    // no collision
    EXPECT_FALSE(gjk(triangle1, 3, quad1, 4));
    // collision
    EXPECT_TRUE(gjk(triangle1, 3, quad2, 4));
}

TEST(GJK, Triangle){

    // exclude
    EXPECT_FALSE(gjk(triangle1, 3, triangle2, 3));
    // intersect
    EXPECT_TRUE(gjk(triangle1, 3, triangle3, 3));
    // include(same)
    EXPECT_TRUE(gjk(triangle1, 3, triangle1, 3));
}

TEST(GJK, Square){
    // exclude
    EXPECT_FALSE(gjk(square1, 4, square3, 4));
    // intersect
    EXPECT_TRUE(gjk(square1, 4, square2, 4));
    // include(same)
    EXPECT_TRUE(gjk(square1, 4, square1, 4));
}

TEST(GJK, TriangleSquare){
    // exclude
    EXPECT_FALSE(gjk(triangle1, 3, square3, 4));
    // intersect
    EXPECT_TRUE(gjk(triangle1, 3, square2, 4));
    // include
    EXPECT_TRUE(gjk(triangle1, 3, square1, 4));
}

TEST(GJK, EdgeCase){
    // empty
    EXPECT_FALSE(gjk(triangle1, 0, square1, 4));
    EXPECT_FALSE(gjk(triangle1, 3, square1, 0));
    EXPECT_FALSE(gjk(triangle1, 0, square1, 0));

    // dot
    Vec2 point1[1] = {{{0, 0}}};
    Vec2 point2[1] = {{{0, 0}}};
    Vec2 point3[1] = {{{1, 1}}};
    Vec2 line1[2] = {{{0, 0}}, {{1, 1}}};
    Vec2 line2[2] = {{{0, 1}}, {{1, 0}}};
    Vec2 line3[2] = {{{1, 0}}, {{2, 1}}};


    // point to point
    EXPECT_TRUE(gjk(point1, 1, point2, 1));
    EXPECT_FALSE(gjk(point1, 1, point3, 1));
    // point to line
    EXPECT_TRUE(gjk(point1, 1, line1, 2));
    EXPECT_TRUE(gjk(point3, 1, line1, 2));
    EXPECT_FALSE(gjk(point1, 1, line2, 2));
    // line to line
    EXPECT_TRUE(gjk(line1, 2, line2, 2));
    EXPECT_FALSE(gjk(line1, 2, line3, 2));
}

TEST(GJK, Touching){
    Vec2 touching_square[4] = {{{1, -1}}, {{3, -1}}, {{3, 1}}, {{1, 1}}};

    EXPECT_TRUE(gjk(square1, 4, touching_square, 4));
}

TEST(GJK, TinyShape){
    Vec2 tiny_triangle[3] = {{{0, 0}}, {{1e-8, 0}}, {{0, 1e-8}}};
    
    EXPECT_TRUE(gjk(tiny_triangle, 3, triangle1, 3));
}

TEST(GJK, ThinShape){
    Vec2 thin_rect[4] = {{{0, 0}}, {{2, 0}}, {{2, 1e-6}}, {{0, 1e-6}}};
    EXPECT_TRUE(gjk(thin_rect, 4, square1, 4));
}

TEST(GJK, ComplexShape){
    Vec2 hexagon[6] = {
        {{ 2, 0}}, {{ 1,  1.73}}, {{-1,  1.73}}, 
        {{-2, 0}}, {{-1, -1.73}}, {{ 1, -1.73}}
    };

    EXPECT_TRUE(gjk(hexagon, 6, square1, 4));
}

TEST(GJK, Performance){
    Vec2 large_poly1[8] = {
        {{0, 0}}, {{2, 0}}, {{ 3, 1}}, {{ 3, 3}}, 
        {{2, 4}}, {{0, 4}}, {{-1, 3}}, {{-1, 1}}
    };
    Vec2 large_poly2[8] = {
        {{1, 1}}, {{3, 1}}, {{4, 2}}, {{4, 4}}, 
        {{3, 5}}, {{1, 5}}, {{0, 4}}, {{0, 2}}
    };

    for(int i = 0; i < 1000; ++i){
        gjk(large_poly1, 8, large_poly2, 8);
    }

    SUCCEED();
}