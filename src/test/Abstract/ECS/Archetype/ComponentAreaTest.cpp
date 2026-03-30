#include "Abstract/ECS/Archetype/ComponentArea.hpp"
#include "Abstract/ECS/Test1.hpp"
#include <gtest/gtest.h>

class MockPool : public IPool {
  public:
	int addEntityCalls = 0;
	int removeLastEntityCalls = 0;
	int moveFromCalls = 0;
	int copyToCalls = 0;

	size_t lastAddReturnedIndex = 0;

	size_t lastMoveFromIndexTo = 0;
	size_t lastMoveFromIndexFrom = 0;

	size_t lastCopyOldIndex = 0;
	size_t lastCopyNewIndex = 0;
	IPool *lastCopyTargetPool = nullptr;
	std::unique_ptr<IPool> createEmpty() { return std::make_unique<MockPool>(); }
	size_t currentSize = 0;

	size_t addEntity() override
	{
		++addEntityCalls;
		lastAddReturnedIndex = currentSize;
		return currentSize++;
	}

	void removeLastEntity() override
	{
		++removeLastEntityCalls;
		if (currentSize > 0) {
			--currentSize;
		}
	}

	void moveFrom(size_t indexTo, size_t indexFrom) override
	{
		++moveFromCalls;
		lastMoveFromIndexTo = indexTo;
		lastMoveFromIndexFrom = indexFrom;
	}

	void copyTo(size_t oldIndex, IPool *otherPool, size_t newIndex) override
	{
		++copyToCalls;
		lastCopyOldIndex = oldIndex;
		lastCopyNewIndex = newIndex;
		lastCopyTargetPool = otherPool;
	}
};

class Test3 : public Component<Test3> {
  public:
	Test3() = default;

	int health{};
	int stamina{};
};

TEST(ComponentArea, registerOneEntity)
{
	ComponentArea componentArea = ComponentArea();
	size_t index = componentArea.registerEntity(EntityID());
	EXPECT_EQ(0, index);
}

TEST(ComponentArea, registerOneEntityAndDeleteIt)
{
	ComponentArea componentArea = ComponentArea();
	EntityID entityId = EntityID();
	size_t index = componentArea.registerEntity(entityId);
	EXPECT_EQ(0, index);
	EXPECT_EQ(entityId, componentArea.removeEntity(index));
	EXPECT_EQ(0, componentArea.entityIds.size());
}

TEST(ComponentArea, registerTwoEntitesDeleteFirstOne)
{
	ComponentArea componentArea = ComponentArea();
	EntityID entityId1 = EntityID();
	size_t index1 = componentArea.registerEntity(entityId1);
	EXPECT_EQ(0, index1);
	EntityID entityId2 = EntityID();
	size_t index2 = componentArea.registerEntity(entityId2);
	EXPECT_EQ(1, index2);
	EntityID lastEntity = componentArea.removeEntity(index1);
	EXPECT_EQ(entityId2, lastEntity);
	auto entityIds = componentArea.entityIds;
	EXPECT_EQ(1, entityIds.size());
	EXPECT_EQ(entityId2, entityIds[0]);
}
TEST(ComponentArea, registerTwoEntitesDeleteSecondOne)
{
	ComponentArea componentArea = ComponentArea();
	EntityID entityId1 = EntityID();
	size_t index1 = componentArea.registerEntity(entityId1);
	EXPECT_EQ(0, index1);
	EntityID entityId2 = EntityID();
	size_t index2 = componentArea.registerEntity(entityId2);
	EXPECT_EQ(1, index2);
	EntityID lastEntity = componentArea.removeEntity(index2);
	EXPECT_EQ(entityId2, lastEntity);
	auto entityIds = componentArea.entityIds;
	EXPECT_EQ(1, entityIds.size());
	EXPECT_EQ(entityId1, entityIds[0]);
}
TEST(ComponentArea, registerThreeEntitesDeleteSecondOne)
{
	ComponentArea componentArea = ComponentArea();
	EntityID entityId1 = EntityID();
	size_t index1 = componentArea.registerEntity(entityId1);
	EXPECT_EQ(0, index1);
	EntityID entityId2 = EntityID();
	size_t index2 = componentArea.registerEntity(entityId2);
	EXPECT_EQ(1, index2);
	EntityID entityId3 = EntityID();
	size_t index3 = componentArea.registerEntity(entityId3);
	EntityID lastEntity = componentArea.removeEntity(index2);
	EXPECT_EQ(entityId3, lastEntity);
	auto entityIds = componentArea.entityIds;
	EXPECT_EQ(2, entityIds.size());
	EXPECT_EQ(entityId1, entityIds[0]);
	EXPECT_EQ(entityId3, entityIds[1]);
}
TEST(ComponentArea, registerThreeEntitesDeleteFirstOne)
{
	ComponentArea componentArea = ComponentArea();
	EntityID entityId1 = EntityID();
	size_t index1 = componentArea.registerEntity(entityId1);
	EXPECT_EQ(0, index1);
	EntityID entityId2 = EntityID();
	size_t index2 = componentArea.registerEntity(entityId2);
	EXPECT_EQ(1, index2);
	EntityID entityId3 = EntityID();
	size_t index3 = componentArea.registerEntity(entityId3);
	EntityID lastEntity = componentArea.removeEntity(index1);
	EXPECT_EQ(entityId3, lastEntity);
	auto entityIds = componentArea.entityIds;
	EXPECT_EQ(2, entityIds.size());
	EXPECT_EQ(entityId3, entityIds[0]);
	EXPECT_EQ(entityId2, entityIds[1]);
}
TEST(ComponentArea, registerThreeEntitesDeleteThirdOne)
{
	ComponentArea componentArea = ComponentArea();
	EntityID entityId1 = EntityID();
	size_t index1 = componentArea.registerEntity(entityId1);
	EXPECT_EQ(0, index1);
	EntityID entityId2 = EntityID();
	size_t index2 = componentArea.registerEntity(entityId2);
	EXPECT_EQ(1, index2);
	EntityID entityId3 = EntityID();
	size_t index3 = componentArea.registerEntity(entityId3);
	EntityID lastEntity = componentArea.removeEntity(index3);
	EXPECT_EQ(entityId3, lastEntity);
	auto entityIds = componentArea.entityIds;
	EXPECT_EQ(2, entityIds.size());
	EXPECT_EQ(entityId1, entityIds[0]);
	EXPECT_EQ(entityId2, entityIds[1]);
}

TEST(ComponentArea, MoveEntityFromOldArchetypeCopiesOnlySharedPools)
{
	ComponentArea oldArea;
	ComponentArea newArea;

	auto sig1 = ArchetypeBitSignature::get<Test1>();
	auto sig2 = ArchetypeBitSignature::get<Test2>();
	auto sig3 = ArchetypeBitSignature::get<Test3>();

	auto oldPool1 = std::make_unique<MockPool>();
	auto oldPool2 = std::make_unique<MockPool>();
	auto newPool2 = std::make_unique<MockPool>();
	auto newPool3 = std::make_unique<MockPool>();

	MockPool *oldPool1Ptr = oldPool1.get();
	MockPool *oldPool2Ptr = oldPool2.get();
	MockPool *newPool2Ptr = newPool2.get();
	MockPool *newPool3Ptr = newPool3.get();

	oldArea.componentPools[sig1] = std::move(oldPool1);
	oldArea.componentPools[sig2] = std::move(oldPool2);

	newArea.componentPools[sig2] = std::move(newPool2);
	newArea.componentPools[sig3] = std::move(newPool3);

	EntityID entity;
	size_t oldIndex = oldArea.registerEntity(entity);

	size_t newIndex = newArea.moveEntityFromOldArchetype(entity, oldIndex, &oldArea);

	EXPECT_EQ(newIndex, 0);

	ASSERT_EQ(newArea.entityIds.size(), 1);
	EXPECT_EQ(newArea.entityIds[0], entity);

	EXPECT_EQ(oldPool1Ptr->copyToCalls, 0);
	EXPECT_EQ(oldPool2Ptr->copyToCalls, 1);

	EXPECT_EQ(oldPool2Ptr->lastCopyOldIndex, oldIndex);
	EXPECT_EQ(oldPool2Ptr->lastCopyNewIndex, newIndex);
	EXPECT_EQ(oldPool2Ptr->lastCopyTargetPool, newPool2Ptr);

	EXPECT_EQ(newPool3Ptr->copyToCalls, 0);
}

TEST(ComponentArea, RegisterEntityAddsEntityToAllPools)
{
	ComponentArea area;

	auto sig1 = ArchetypeBitSignature::get<Test1>();
	auto sig2 = ArchetypeBitSignature::get<Test2>();

	auto pool1 = std::make_unique<MockPool>();
	auto pool2 = std::make_unique<MockPool>();

	MockPool *pool1Ptr = pool1.get();
	MockPool *pool2Ptr = pool2.get();

	area.componentPools[sig1] = std::move(pool1);
	area.componentPools[sig2] = std::move(pool2);

	EntityID entity;
	size_t index = area.registerEntity(entity);

	EXPECT_EQ(index, 0);
	EXPECT_EQ(pool1Ptr->addEntityCalls, 1);
	EXPECT_EQ(pool2Ptr->addEntityCalls, 1);
}
