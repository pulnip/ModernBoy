#include <gtest/gtest.h>
#include "memory/object_pool.hpp"

using ModernBoy::ObjectPool;

struct RAII {
    static int count;
    RAII(){
        ++count;
    }
    RAII(const RAII&){
        ++count;
    }
    RAII(RAII&&) noexcept{
        ++count;
    }
    ~RAII(){
        --count;
    }
};
int RAII::count = 0;

struct Entity {
    int id;
    Entity(int i):id(i){}
    bool operator==(const Entity& o) const{ return id == o.id; }
};

TEST(ObjectPool, ReserveWithValue){
    ObjectPool<int> pool;
    pool.reserve(5);
    EXPECT_EQ(pool.capacity(), 5);
    EXPECT_EQ(pool.size(), 0);
}

TEST(ObjectPool, CreateAndDestroyWithValue){
    ObjectPool<int> pool;
    // emplace
    auto handle1 = pool.emplace(10);
    EXPECT_EQ(pool[handle1], 10);
    // push
    auto handle2 = pool.push(20);
    EXPECT_EQ(pool[handle2], 20);
    EXPECT_EQ(pool.size(), 2);

    pool.remove(handle1);
    EXPECT_EQ(pool.size(), 1);

    // slot reuse
    auto handle3 = pool.emplace(30);
    EXPECT_EQ(handle3.index, handle1.index);
    EXPECT_EQ(pool[handle3], 30);
}

TEST(ObjectPool, LifecycleWithRAII){
    RAII::count = 0;
    {
        ObjectPool<RAII> pool;
        EXPECT_EQ(RAII::count, 0);

        auto handle1 = pool.emplace();
        EXPECT_EQ(RAII::count, 1);
        EXPECT_EQ(RAII::count, pool.size());
        auto _ = pool.emplace();
        EXPECT_EQ(RAII::count, 2);
        EXPECT_EQ(RAII::count, pool.size());

        pool.remove(handle1);
        EXPECT_EQ(RAII::count, 1);
        EXPECT_EQ(RAII::count, pool.size());
    }
    EXPECT_EQ(RAII::count, 0);
}

TEST(ObjectPool, CreateAndReadWithEntity) {
    ObjectPool<Entity> pool;
    auto handle1 = pool.emplace(1);
    EXPECT_EQ(pool[handle1].id, 1);

    Entity tmp(2);
    auto handle2 = pool.push(std::move(tmp));
    EXPECT_EQ(pool[handle2].id, 2);

    EXPECT_EQ(pool.size(), 2);
}

TEST(ObjectPool, RemoveAndReuseWithEntity) {
    ObjectPool<Entity> pool;
    auto handle1 = pool.emplace(100);
    auto _ = pool.emplace(200);
    EXPECT_EQ(pool.size(), 2);

    pool.remove(handle1);
    EXPECT_EQ(pool.size(), 1);

    // reuse
    auto handle3 = pool.emplace(300);
    EXPECT_EQ(handle3.index, handle1.index);
    EXPECT_EQ(pool[handle3].id, 300);
}


TEST(ObjectPool, ConstAccess){
    ObjectPool<int> pool;
    auto handle = pool.emplace(42);
    const auto& cref = pool;
    EXPECT_EQ(cref[handle], 42);
    pool.remove(handle);
    EXPECT_THROW(cref[handle], std::out_of_range);
}
