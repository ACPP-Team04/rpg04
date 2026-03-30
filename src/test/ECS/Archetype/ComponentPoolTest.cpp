#include "Abstract/ECS/Archetype/ComponentPool.hpp"

#include "../TestComponents.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"

#include <gtest/gtest.h>
static int N = 10;

TEST(ComponentPoolTest, addEntity)
{
	ComponentPool<Intcomp1> component_pool;
	for (int i = 0; i < N; i++) {
		component_pool.addEntity();
	}
	EXPECT_EQ(component_pool.components.size(), N);
}

TEST(ComponentPoolTest, removeEntity)
{
	ComponentPool<Intcomp1> component_pool;
	for (int i = 0; i < N; i++) {
		component_pool.addEntity();
	}
	component_pool.removeLastEntity();
	EXPECT_EQ(component_pool.components.size(), N - 1);
}
TEST(ComponentPoolTest, removeAllEntitiesPlus1)
{
	ComponentPool<Intcomp1> component_pool;
	for (int i = 0; i < N; i++) {
		component_pool.addEntity();
	}
	for (int i = 0; i < N; i++) {
		component_pool.removeLastEntity();
	}
	EXPECT_THROW(component_pool.removeLastEntity(), std::range_error);
}

TEST(ComponentPoolTest, getCorrectComponent)
{
	ComponentPool<Intcomp1> component_pool;
	for (int i = 0; i < N; i++) {
		component_pool.addEntity();
		component_pool.components[i].member = i;
	}
	for (int i = 0; i < N; i++) {
		Intcomp1 component = component_pool.getComponent(i);
		EXPECT_EQ(component.member, i);
	}
}

TEST(ComponentPoolTest, getComponentFromEmptyVector)
{
	ComponentPool<Intcomp1> component_pool;
	for (int i = 0; i < N; i++) {
		component_pool.addEntity();
		component_pool.components[i].member = i;
	}
	for (int i = 0; i < N; i++) {
		component_pool.removeLastEntity();
	}
	EXPECT_THROW(component_pool.getComponent(1), std::range_error);
}
TEST(ComponentPoolTest, moveFrom)
{
	ComponentPool<Intcomp1> component_pool;
	for (int i = 0; i < N; i++) {
		component_pool.addEntity();
		component_pool.components[i].member = i;
	}
	component_pool.moveFrom(0, N - 1);
	EXPECT_EQ(component_pool.components.size(), N);
	EXPECT_EQ(component_pool.getComponent(N - 1).member, N - 1);
	EXPECT_EQ(component_pool.getComponent(0).member, N - 1);
}

TEST(ComponentPoolTest, moveFromIndexOutOfBounds)
{
	ComponentPool<Intcomp1> component_pool;
	EXPECT_THROW(component_pool.moveFrom(0, 0), std::range_error);
}

TEST(ComponentPoolTest, copyTo)
{
	int first = 0;
	int value = 1;
	ComponentPool<Intcomp1> component_pool;
	ComponentPool<Intcomp1> *newPool = new ComponentPool<Intcomp1>();

	component_pool.addEntity();
	component_pool.components[first].member = value + 1;
	newPool->addEntity();
	newPool->components[first].member = value;
	EXPECT_EQ(component_pool.components[first].member, value + 1);
	EXPECT_EQ(newPool->components[first].member, value);
	component_pool.copyTo(first, newPool, first);
	EXPECT_EQ(component_pool.components[first].member, value + 1);
	EXPECT_EQ(newPool->components[first].member, value + 1);
}

/*
*#pragma once
#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>

struct IPool {
    IPool() = default;
    virtual ~IPool() = default;
    IPool(const IPool &other) = default;
    IPool(IPool &&other) = default;
    IPool &operator=(const IPool &other) = default;
    IPool &operator=(IPool &&other) = default;
    virtual size_t addEntity() = 0;
    virtual void removeLastEntity() = 0;
    virtual void moveFrom(size_t indexTo, size_t indexFrom) = 0;
    virtual void copyTo(size_t oldIndex, IPool *otherPool, size_t newIndex) = 0;
};

template <typename T>
struct ComponentPool : IPool {
    std::vector<T> components;

    size_t addEntity() override
    {
        this->components.emplace_back();
        return this->components.size() - 1;
    }

    void removeLastEntity() override { this->components.pop_back(); }

    T &getComponent(size_t location) { return this->components[location]; }
    T &getLastEntityComponent() { return this->components.back(); }

    void moveFrom(size_t indexTo, size_t indexFrom) override
    {
        this->components[indexTo] = std::move(this->components[indexFrom]);
    }

    void copyTo(size_t oldIndex, IPool *newPool, size_t newIndex) override
    {
        std::cout << oldIndex << " " << newIndex << std::endl;
        auto *newPoolCast = static_cast<ComponentPool<T> *>(newPool);
        newPoolCast->components[newIndex] = std::move(this->components[oldIndex]);
    }
};
 */