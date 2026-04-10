#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"

#include "../../../ECS/TestComponents.hpp"
#include "Abstract/ECS/Test1.hpp"

#include <Abstract/Overwordl/Components/MovementComponent.hpp>
#include <Abstract/Overwordl/Components/Player_Component.hpp>
#include <Abstract/Utils/WorldUtlis.hpp>
#include <Implementation/Components/BattleComponent.hpp>
#include <Implementation/Components/StatsComponent.hpp>
#include <gtest/gtest.h>

TEST(ArchetypeManager, createEntityWIthNewArchetype)
{
	ArchetypeManager archetypeManager = ArchetypeManager();
	EntityID entity_id1 = archetypeManager.createEntity<Intcomp1, Intcomp2>();

	Intcomp1 &intcomp1 = archetypeManager.getComponent<Intcomp1>(entity_id1);
	Intcomp2 &intcomp2 = archetypeManager.getComponent<Intcomp2>(entity_id1);

	int counter = 0;
	archetypeManager.view<Intcomp1, Intcomp2>().each([&](EntityID entityId, Intcomp1 &intcomp1, Intcomp2 intcomp2) {
		if (entityId == entity_id1) {
			counter += 1;
		}
	});
	EXPECT_EQ(1, counter);
}

TEST(ArchetypeManager, createEntityWIthSameArchetype)
{
	ArchetypeManager archetypeManager = ArchetypeManager();
	EntityID entity_id1 = archetypeManager.createEntity<Intcomp1, Intcomp2>();
	archetypeManager.addComponentToEntity<Intcomp1, Intcomp2>(entity_id1);

	int counter = 0;
	archetypeManager.view<Intcomp1, Intcomp2>().each([&](EntityID entityId, Intcomp1 &intcomp1, Intcomp2 intcomp2) {
		if (entityId == entity_id1) {
			counter += 1;
		}
	});
	EXPECT_EQ(1, counter);
}

TEST(ArchetypeManager, createEntityWithNewArchetype)
{
	ArchetypeManager archetypeManager = ArchetypeManager();
	EntityID entity_id1 = archetypeManager.createEntity<Intcomp1, Intcomp2>();
	EntityID entity_id2 = archetypeManager.createEntity<Intcomp1, Intcomp2, Doublecomp5>();
	archetypeManager.addComponentToEntity<Intcomp1, Intcomp2, Intcomp3>(entity_id1);
	archetypeManager.addComponentToEntity<Doublecomp4>(entity_id1);
	int counter = 0;
	archetypeManager.view<Intcomp1, Intcomp2>().each([&](EntityID id, auto &, auto &) { counter++; });

	EXPECT_EQ(counter, 2);
	counter = 0;
	archetypeManager.view<Intcomp1, Intcomp2, Doublecomp4>().each(
	    [&](EntityID id, auto &, auto &, auto &) { counter++; });
	EXPECT_EQ(counter, 1);
}

TEST(ArchetypeManager, removeComponentFromEntity)
{
	ArchetypeManager archetypeManager = ArchetypeManager();
	EntityID entity_id1 = archetypeManager.createEntity<Intcomp1, Intcomp2>();
	EntityID entity_id2 = archetypeManager.createEntity<Intcomp1, Intcomp2, Doublecomp5>();

	archetypeManager.removeComponentFromEntity<Intcomp1, Intcomp2>(entity_id1);
	archetypeManager.removeComponentFromEntity<Intcomp1, Intcomp2>(entity_id2);
	int counter = 0;
	archetypeManager.view<Intcomp1, Intcomp2>().each([&](EntityID id, auto &, auto &) { counter++; });
	EXPECT_EQ(0, counter);
	counter = 0;
	archetypeManager.view<Doublecomp5>().each([&](EntityID id, auto &) { counter++; });
	EXPECT_EQ(1, counter);
}

TEST(ArchetypeManager, testQuery)
{
	ArchetypeManager archetypeManager = ArchetypeManager();
	EntityID entity_id1 = archetypeManager.createEntity<Intcomp1, Intcomp2>();

	Intcomp1 &intcomp1 = archetypeManager.getComponent<Intcomp1>(entity_id1);
	intcomp1.member = 1;
	Intcomp2 &intcomp2 = archetypeManager.getComponent<Intcomp2>(entity_id1);
	intcomp2.member = 2;
	int sum = 0;
	archetypeManager.view<Intcomp1, Intcomp2>().each([&](EntityID entityId, Intcomp1 &intcomp1, Intcomp2 intcomp2) {
		sum = sum + intcomp1.member + intcomp2.member;
	});
	EXPECT_EQ(3, sum);
}

TEST(ArchetypeManager, addMultiple)
{
	ArchetypeManager archetypeManager = ArchetypeManager();
	EntityID entity_id1 = archetypeManager.createEntity<>();

	archetypeManager.addComponentToEntity<Test1>(entity_id1);
	archetypeManager.addComponentToEntity<Test2>(entity_id1);
	EntityID entity_id2 = archetypeManager.createEntity<>();

	archetypeManager.addComponentToEntity<Test1>(entity_id2);
	archetypeManager.addComponentToEntity<Test2>(entity_id2);

	EntityID entity_id3 = archetypeManager.createEntity<>();

	archetypeManager.addComponentToEntity<Test1>(entity_id3);
	archetypeManager.addComponentToEntity<Test2>(entity_id3);
	int counter = 0;
	archetypeManager.view<Test1, Test2>().each([&](EntityID entityId, Test1 &test1, Test2 &test2) { counter++; });
	EXPECT_EQ(3, counter);
}

TEST(ArchetypeManagerTest, ProvenDataCorruption)
{
	spdlog::set_level(spdlog::level::debug);
	ArchetypeManager manager;

	std::vector<EntityID> dummies;
	for (int i = 0; i < 100; ++i) {
		dummies.push_back(manager.createEntity<StatsComponent, BattleComponent, MovementComponent>());
	}

	auto playerID = manager.createEntity<StatsComponent, PlayerComponent, MovementComponent>();
	spdlog::get("combat")->debug("Created player with ID: {}", playerID.getId());
	EXPECT_EQ(true, WorldUtils::getPlayer(manager).has_value());
	auto player = WorldUtils::getPlayer(manager);
	EntityID playerId;
	if (player.has_value()) {
		playerId = player.value();
		manager.addComponentToEntity<BattleComponent>(playerId);
		auto bcId = manager.getComponent<BattleComponent>(player.value());
		manager.removeComponentFromEntity<BattleComponent>(playerId);
		manager.getComponent<MovementComponent>(playerId).speed = 3.0f;
		manager.addComponentToEntity<BattleComponent>(playerId);
		manager.removeComponentFromEntity<BattleComponent>(playerId);

	} else {
		FAIL() << "Player was not created successfully!";
	}
	player = WorldUtils::getPlayer(manager);
	EXPECT_EQ(true, WorldUtils::getPlayer(manager).has_value());
}
