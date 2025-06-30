#include <bit>
#include <gtest/gtest.h>
#include "util/dynamic_vector.hpp"

using namespace ModernBoy;

TEST(DynamicVectorMemory, Trivial){
    for(size_t c=4; c<=100; c+=4){
        DynamicVector vec(c);
        const auto& ref = vec;

        EXPECT_EQ(vec.getChunkSize(), c);
        EXPECT_EQ(ref.getChunkSize(), c);

        EXPECT_EQ(vec.size(), 0);
        EXPECT_EQ(ref.size(), 0);
        EXPECT_EQ(vec.begin(), vec.end());
        EXPECT_EQ(ref.begin(), ref.end());
        EXPECT_EQ(vec.cbegin(), vec.end());
    }
}

TEST(DynamicVectorMemory, LinearlyGrowth){
    for(size_t c=4; c<=100; c+=4){
        DynamicVector vec(c);
        const auto& ref = vec;

        for(size_t i=0; i<10; ++i){
            vec.newChunk(1);
            EXPECT_EQ(vec.size(), i+1);
            EXPECT_EQ(ref.size(), i+1);
        }

        auto it1 = vec.begin();
        auto it2 = ref.begin();
        auto it3 = vec.cbegin();
        for(size_t i=0; i<10; ++i){
            ++it1;
            ++it2;
            ++it3;
        }
        EXPECT_EQ(it1, vec.end());
        EXPECT_EQ(it2, ref.end());
        EXPECT_EQ(it3, vec.cend());
    }
}

TEST(DynamicVectorMemory, LinearlyGraduallyGrowth){
    for(size_t c=4; c<=100; c+=4){
        DynamicVector vec(c);
        const auto& ref = vec;

        size_t sum = 0;
        for(size_t i=1; i<=10; ++i){
            vec.newChunk(i);
            sum += i;
            EXPECT_EQ(vec.size(), sum);
            EXPECT_EQ(ref.size(), sum);
        }
    }
}

TEST(DynamicVectorValue, Trivlal){
    for(int32_t i=1; i<=25; ++i){
        DynamicVector vec(4*i);
        const auto& ref = vec;
        vec.newChunk(1);

        auto ptr1 = vec[0];
        for(int32_t j=0; j<i; ++j){
            int32_t arr[]={j};
            memcpy(ptr1, arr, 4);
            ptr1 = Util::add(ptr1, 4);
        }

        auto ptr2 = ref[0];
        for(int32_t j=0; j<i; ++j){
            int32_t val = -1;
            memcpy(&val, ptr2, 4);
            EXPECT_EQ(val, j);
            ptr2 = Util::add(ptr2, 4);
        }
    }
}

TEST(DynamicVectorMemory, Reuse){
    for(size_t i=2; i<=100; ++i){
        DynamicVector vec(4);
        vec.newChunk(2*i);

        for(Index j=0; j<i; ++j)
            vec.freeChunk(2*j, 1);
        for(Index j=0; j<i; ++j)
            vec.newChunk(1);

        EXPECT_EQ(vec.size(), 2*i);
        EXPECT_EQ(vec.capacity(), std::bit_ceil(2*i));
    }
    for(size_t i=2; i<=100; ++i){
        DynamicVector vec(4, 2*i);

        for(Index j=0; j<i; ++j)
            vec.freeChunk(2*j, 1);
        for(Index j=0; j<i; ++j)
            vec.newChunk(1);

        EXPECT_EQ(vec.size(), 2*i);
        EXPECT_EQ(vec.capacity(), std::bit_ceil(2*i));
    }
}

