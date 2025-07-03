#include <bit>
#include <gtest/gtest.h>
#include "util/dynamic_vector.hpp"

using namespace ModernBoy;

TEST(DynamicVectorMemory, Trivial){
    for(size_t c=4; c<=100; c+=4){
        DynamicVector vec(c);
        const auto& ref = vec;

        EXPECT_EQ(vec.elmSize(), c);
        EXPECT_EQ(ref.elmSize(), c);

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
            vec.insertRange(1);
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
            vec.insertRange(i);
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
        vec.reserve(1);

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
        vec.insertRange(2*i);

        for(Index j=0; j<i; ++j)
            vec.remove(2*j, 1);
        for(Index j=0; j<i; ++j)
            vec.insertRange(1);

        EXPECT_EQ(vec.size(), 2*i);
        EXPECT_EQ(vec.capacity(), std::bit_ceil(2*i));
    }
    for(size_t i=2; i<=100; ++i){
        DynamicVector vec(4, 2*i);

        for(Index j=0; j<i; ++j)
            vec.remove(2*j, 1);
        for(Index j=0; j<i; ++j)
            vec.insertRange(1);

        EXPECT_EQ(vec.size(), 2*i);
        EXPECT_EQ(vec.capacity(), std::bit_ceil(2*i));
    }
}

TEST(DynamicVectorIterator, Trivial){
    for(size_t c=4; c<=100; ++c){
        DynamicVector vec(4, c);

        for(int32_t i=0; i<vec.size(); ++i)
            memcpy(vec[i], &i, 4);

        size_t count=0;
        for(const auto& v: vec){
            int32_t x = -1;
            memcpy(&x, v.elmMem, 4);
            EXPECT_EQ(x, count++);
        }
        EXPECT_EQ(count, vec.size());
    }
}

TEST(DynamicVectorIterator, SkipFreed){
    for(size_t c=4; c<=100; ++c){
        DynamicVector vec(4, 2*c);

        vec.remove(c, c);

        size_t count=0;
        for(const auto& _: vec){
            ++count;
        }

        EXPECT_EQ(count, c);
        EXPECT_EQ(vec.size(), c);
    }
    for(size_t c=4; c<=100; ++c){
        DynamicVector vec(4, 2*c);

        for(size_t i=c; i<2*c; ++i)
            vec.remove(i, 1);

        size_t count=0;
        for(const auto& _: vec){
            ++count;
        }

        EXPECT_EQ(count, c);
    }
}

TEST(DynamicVectorPart, Trivial){
    uint64_t x = 42;
    float y = 3.14;
    double z = 1.414;
    char w = 'w';
    constexpr auto elmSize = sizeof(x)+sizeof(y)+sizeof(z)+sizeof(w);

    DynamicVector vec(elmSize, 1);
    vec.set(0, 0, x, y, z, w);

    uint64_t a = 0;
    float b = 0;
    double c = 0;
    char d = '\0';
    vec.get(0, 0, a, b, c, d);

    EXPECT_EQ(x, a);
    EXPECT_EQ(y, b);
    EXPECT_EQ(z, c);
    EXPECT_EQ(w, d);
    EXPECT_ANY_THROW(vec.set(1, 0, x, y, z, w));
    EXPECT_ANY_THROW(vec.get(1, 0, x, y, z, w));

    a = b = c = d = 0;
    vec.get(0, 0, a);
    vec.get(0, sizeof(a), b);
    vec.get(0, sizeof(a)+sizeof(b), c);
    vec.get(0, sizeof(a)+sizeof(b)+sizeof(c), d);
    EXPECT_EQ(x, a);
    EXPECT_EQ(y, b);
    EXPECT_EQ(z, c);
    EXPECT_EQ(w, d);
}

TEST(DynamicVectorPart, MisPartitioning){
    uint64_t x = 42;
    float y = 3.14;
    double z = 1.414;
    char w = 'w';
    constexpr auto elmSize = sizeof(x)+sizeof(y)+sizeof(z)+sizeof(w);

    DynamicVector vec(elmSize, 2);
    EXPECT_ANY_THROW(vec.set(0, 1, x, y, z, w));
    EXPECT_ANY_THROW(vec.get(0, 1, x, y, z, w));
    EXPECT_ANY_THROW(vec.set(1, 1, x, y, z, w));
    EXPECT_ANY_THROW(vec.get(1, 1, x, y, z, w));
}

TEST(DynamicVectorPart, InsertMiddle){
    uint64_t x = 42;
    float y = 3.14;
    double z = 1.414;
    char w = 'w';
    constexpr auto elmSize = sizeof(x)+sizeof(y)+sizeof(z)+sizeof(w);
    for(size_t s=1; s<=100; ++s){
        DynamicVector vec(elmSize, s);
        for(size_t i=0; i<s; ++i){
            vec.set(i, 0, x, y, z, w);

            uint64_t a = 0;
            float b = 0;
            double c = 0;
            char d = '\0';
            vec.get(i, 0, a, b, c, d);

            EXPECT_EQ(x, a);
            EXPECT_EQ(y, b);
            EXPECT_EQ(z, c);
            EXPECT_EQ(w, d);

            vec.remove(i, 1);
            EXPECT_ANY_THROW(vec.set(i, 0, x, y, z, w));
            EXPECT_ANY_THROW(vec.get(i, 0, x, y, z, w));
        }
    }
}

TEST(DynamicVectorPart, SparseVector){
    uint64_t x = 42;
    float y = 3.14;
    double z = 1.414;
    char w = 'w';
    constexpr auto elmSize = sizeof(x)+sizeof(y)+sizeof(z)+sizeof(w);

    for(size_t s=1; s<=100; ++s){
        for(size_t i=0; i<s; ++i){
            DynamicVector vec(elmSize, s);

            if(i > 0)
                vec.remove(0, i);
            if(i+1 < s)
                vec.remove(i+1, s-(i+1));
            for(size_t j=0; j<s; ++j){
                if(i==j){
                    vec.set(j, 0, x, y, z, w);
                    uint64_t a = 0;
                    float b = 0;
                    double c = 0;
                    char d = '\0';
                    vec.get(j, 0, a, b, c, d);
                }
                else{
                    EXPECT_ANY_THROW(vec.set(j, 0, x, y, z, w));
                    EXPECT_ANY_THROW(vec.get(j, 0, x, y, z, w));
                }
            }
        }
    }
}