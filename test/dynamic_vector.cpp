#include <optional>
#include <vector>
#include <gtest/gtest.h>
#include "memory/dynamic_vector.hpp"

using namespace ModernBoy;

TEST(DynamicVector, ZeroChunkBehaviors){
    DynamicVector vec(0);
    EXPECT_EQ(vec.size(), 0u);
    EXPECT_EQ(vec.capacity(), 0u);
    vec.clear();
    EXPECT_EQ(vec.size(), 0u);

    EXPECT_DEATH(vec[0], "");
}

TEST(DynamicVector, ReserveAndResize){
    DynamicVector vec(sizeof(int));
    EXPECT_EQ(vec.size(), 0u);
    EXPECT_EQ(vec.capacity(), 0u);

    vec.reserve(4);
    EXPECT_GE(vec.capacity(), 4u);

    vec.resize(4);
    EXPECT_EQ(vec.size(), 4u);
    EXPECT_GE(vec.capacity(), 4u);
}

TEST(DynamicVector, EmplaceAndAccess){
    DynamicVector vec(sizeof(int));
    vec.emplace(10);
    vec.emplace(20);
    EXPECT_EQ(vec.size(), 2u);

    int* p0 = static_cast<int*>(vec[0]);
    int* p1 = static_cast<int*>(vec[1]);
    EXPECT_EQ(*p0, 10);
    EXPECT_EQ(*p1, 20);
}

TEST(DynamicVector, SwapRemoveShrinksAndMoves){
    DynamicVector vec(sizeof(int));
    vec.emplace(1);
    vec.emplace(2);
    vec.emplace(3);
    ASSERT_EQ(vec.size(), 3u);

    vec.swap_remove(1);
    EXPECT_EQ(vec.size(), 2u);

    int* p1 = static_cast<int*>(vec[1]);
    EXPECT_EQ(*p1, 3);
}

TEST(DynamicVector, ClearResetsSize){
    DynamicVector vec(sizeof(int));
    vec.emplace(5);
    vec.emplace(6);
    EXPECT_EQ(vec.size(), 2u);

    vec.clear();
    EXPECT_EQ(vec.size(), 0u);
    EXPECT_GE(vec.capacity(), 2u);
}

TEST(DynamicVector, Iterator){
    DynamicVector vec(sizeof(int));
    vec.emplace(7);
    vec.emplace(8);
    vec.emplace(9);
    ASSERT_EQ(vec.size(), 3u);

    std::vector<int> results;
    for (auto it = vec.begin(); it != vec.end(); ++it){
        void* raw = *it;
        int value = *static_cast<int*>(raw);
        results.push_back(value);
    }

    std::vector<int> expected = {7, 8, 9};
    EXPECT_EQ(results, expected);
}


// Composite chunk (int, float, char[4]) tests
TEST(DynamicVector, CompositeChunkSingleElement){
    size_t chunkSize = sizeof(int) + sizeof(float) + 4 * sizeof(char);
    DynamicVector vec(chunkSize);
    vec.emplace(42, 2.718f, 'h', 'e', 'l', 'o');
    ASSERT_EQ(vec.size(), 1u);

    void* raw = vec[0];
    int i = *static_cast<int*>(raw);
    float f = *reinterpret_cast<float*>(static_cast<char*>(raw) + sizeof(int));
    char* chars = static_cast<char*>(raw) + sizeof(int) + sizeof(float);

    EXPECT_EQ(i, 42);
    EXPECT_FLOAT_EQ(f, 2.718f);
    EXPECT_EQ(chars[0], 'h');
    EXPECT_EQ(chars[1], 'e');
    EXPECT_EQ(chars[2], 'l');
    EXPECT_EQ(chars[3], 'o');
}

TEST(DynamicVector, CompositeChunkMultipleElements){
    size_t chunkSize = sizeof(int) + sizeof(float) + 4 * sizeof(char);
    DynamicVector vec(chunkSize);
    vec.reserve(3);
    vec.emplace(1, 1.1f, 'a', 'b', 'c', 'd');
    vec.emplace(2, 2.2f, 'e', 'f', 'g', 'h');
    vec.emplace(3, 3.3f, 'i', 'j', 'k', 'l');
    ASSERT_EQ(vec.size(), 3u);

    for (size_t idx = 0; idx < vec.size(); ++idx){
        void* raw = vec[idx];
        int expected_i = static_cast<int>(idx) + 1;
        float expected_f = expected_i * 1.1f;
        EXPECT_EQ(*static_cast<int*>(raw), expected_i);
        EXPECT_FLOAT_EQ(
            *reinterpret_cast<float*>(static_cast<char*>(raw) + sizeof(int)),
            expected_f
        );
        char* chars = static_cast<char*>(raw) + sizeof(int) + sizeof(float);
        EXPECT_EQ(chars[0], static_cast<char>('a' + idx * 4));
        EXPECT_EQ(chars[1], static_cast<char>('b' + idx * 4));
        EXPECT_EQ(chars[2], static_cast<char>('c' + idx * 4));
        EXPECT_EQ(chars[3], static_cast<char>('d' + idx * 4));
    }
}

TEST(DynamicVector, CompositeChunkSwapRemove){
    size_t chunkSize = sizeof(int) + sizeof(float) + 4 * sizeof(char);
    DynamicVector vec(chunkSize);
    vec.emplace(10, 10.1f, 'x', 'y', 'z', 'w');
    vec.emplace(20, 20.2f, 'u', 'v', 'w', 'x');
    vec.emplace(30, 30.3f, 'q', 'r', 's', 't');
    ASSERT_EQ(vec.size(), 3u);

    vec.swap_remove(1);
    ASSERT_EQ(vec.size(), 2u);

    void* raw = vec[1];
    EXPECT_EQ(*static_cast<int*>(raw), 30);
    EXPECT_FLOAT_EQ(
        *reinterpret_cast<float*>(static_cast<char*>(raw) + sizeof(int)),
        30.3f
    );
    char* chars = static_cast<char*>(raw) + sizeof(int) + sizeof(float);
    EXPECT_EQ(chars[0], 'q');
    EXPECT_EQ(chars[1], 'r');
    EXPECT_EQ(chars[2], 's');
    EXPECT_EQ(chars[3], 't');
}

TEST(DynamicVector, PointerEmplaceAllNonNull){
    size_t chunkSize = sizeof(int) * 2;
    DynamicVector vec(chunkSize);
    int a = 100, b = 200;
    vec.emplace(&a, &b);
    ASSERT_EQ(vec.size(), 1u);

    int* data = static_cast<int*>(vec[0]);
    EXPECT_EQ(data[0], a);
    EXPECT_EQ(data[1], b);
}

TEST(DynamicVector, PointerEmplaceSkipFirst){
    size_t chunkSize = sizeof(int) * 2;
    DynamicVector vec(chunkSize);
    int a = 100, b = 200;
    vec.emplace(&a, nullptr, &b, nullptr);
    ASSERT_EQ(vec.size(), 1u);

    int* data = static_cast<int*>(vec[0]);
    EXPECT_EQ(data[0], a);
    EXPECT_EQ(data[1], b);
}

TEST(DynamicVector, OptionalEmplaceAllPresent){
    size_t chunkSize = sizeof(int) * 2;
    DynamicVector vec(chunkSize);
    std::optional<int> a = 300, b = 400;
    vec.emplace(a, b);
    ASSERT_EQ(vec.size(), 1u);

    int* data = static_cast<int*>(vec[0]);
    EXPECT_EQ(data[0], *a);
    EXPECT_EQ(data[1], *b);
}

TEST(DynamicVector, OptionalEmplaceSkipSecond){
    size_t chunkSize = sizeof(int) * 2;
    DynamicVector vec(chunkSize);
    std::optional<int> a = 300, b = 400, x = std::nullopt;
    vec.emplace(x, a, x, b);
    ASSERT_EQ(vec.size(), 1u);

    int* data = static_cast<int*>(vec[0]);
    EXPECT_EQ(data[0], *a);
}