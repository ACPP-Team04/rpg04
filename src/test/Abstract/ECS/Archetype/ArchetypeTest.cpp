#include "Abstract/ECS/Archetype/Archetype.hpp"
#include "Abstract/ECS/Test1.hpp"
#include <gtest/gtest.h>

class Test3 : public Component<Test3> {
  public:
	Test3() = default;

	int x{};
	int y{};

	void readFromJson(const nlohmann::json &j) override {}
};

TEST(Archetype, CreateNew)
{
	auto archetype = Archetype::createArchType<Test1, Test2>();
	EXPECT_NE(archetype, nullptr);
	auto expectedArcheTypeSignature = ArchetypeBitSignature::get<Test1, Test2>();
	EXPECT_EQ(archetype->getArchTypeSignature(), expectedArcheTypeSignature);
}

TEST(Archetype, CreateArchTypeCreatesComponentPools)
{
	auto archetype = Archetype::createArchType<Test1, Test2>();
	auto *area = archetype->getComponentArea();

	EXPECT_TRUE(area->componentPools.contains(ArchetypeBitSignature::get<Test1>()));
	EXPECT_TRUE(area->componentPools.contains(ArchetypeBitSignature::get<Test2>()));
}

TEST(Archetype, RegisterEntityAddsEntityToComponentArea)
{
	auto archetype = Archetype::createArchType<Test1>();
	size_t sizeBefore = archetype->getComponentArea()->entityIds.size();
	EntityID entity;
	size_t index = archetype->registerEntity(entity);

	EXPECT_EQ(index, 0);
	ASSERT_EQ(archetype->getComponentArea()->entityIds.size(), 1);
	EXPECT_EQ(archetype->getComponentArea()->entityIds[0], entity);
}

TEST(Archetype, RemoveEntityRemovesFromComponentArea)
{
	auto archetype = Archetype::createArchType<Test1>();

	EntityID e1, e2;
	archetype->registerEntity(e1);
	archetype->registerEntity(e2);
	size_t sizeBefore = archetype->getComponentArea()->entityIds.size();
	EntityID returned = archetype->removeEntity(0);

	EXPECT_EQ(returned, e2);
	ASSERT_EQ(archetype->getComponentArea()->entityIds.size(), sizeBefore - 1);
	EXPECT_EQ(archetype->getComponentArea()->entityIds[0], e2);
}

TEST(Archetype, MoveComponentsFromArchetypeRegistersEntityInTarget)
{
	auto oldArchetype = Archetype::createArchType<Test1, Test2>();
	auto newArchetype = Archetype::createArchType<Test2, Test3>();

	EntityID entity;
	size_t oldIndex = oldArchetype->registerEntity(entity);

	size_t newIndex = newArchetype->moveComponentsFromArchetype(entity, oldIndex, oldArchetype);
	size_t sizeBefore = newArchetype->getComponentArea()->entityIds.size();

	EXPECT_EQ(newIndex, 0);
	ASSERT_EQ(newArchetype->getComponentArea()->entityIds.size(), sizeBefore);
	EXPECT_EQ(newArchetype->getComponentArea()->entityIds[0], entity);
}

TEST(Archetype, GetComponentAreaReturnsNonNullPointer)
{
	auto archetype = Archetype::createArchType<Test1>();
	EXPECT_NE(archetype->getComponentArea(), nullptr);
}
