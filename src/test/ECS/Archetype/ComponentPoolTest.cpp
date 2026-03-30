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