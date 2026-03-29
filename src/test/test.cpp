#include "../main/Abstract/ECS/Component/Component.hpp"
#include "Abstract/ECS/Component/ComponentTypeInfo.hpp"
#include "Abstract/ECS/Test1.hpp"
#include <Abstract/ECS/Archetype/ArchetypeManager.hpp>
#include <gtest/gtest.h>

// problem with global state of entity counter, because one tests run before, meaning that the ids are shifted by +1
TEST(ArchtypeMangerCreateEntity, DISABLED_MyTest)
{

	ArchetypeManager archtypeManger = ArchetypeManager();

	EntityID entity_id = archtypeManger.createEntity<Test1>();
	EntityID entity_id2 = archtypeManger.createEntity<Test2>();
	EntityID entity_id3 = archtypeManger.createEntity<Test1, Test2, Test1>();
	EXPECT_EQ(entity_id.getId(), 0);
	EXPECT_EQ(entity_id2.getId(), 1);
	EXPECT_EQ(entity_id3.getId(), 2);

	archtypeManger.addComponent<Test1, Test2>(entity_id);
	EXPECT_EQ(entity_id3.getId(), 2);
}
