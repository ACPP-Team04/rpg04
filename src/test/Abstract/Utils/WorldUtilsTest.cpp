#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/Test1.hpp"

#include <Abstract/Overwordl/Components/InputComponent.hpp>
#include <Abstract/Overwordl/Components/Player_Component.hpp>
#include <Abstract/Utils/WorldUtlis.hpp>
#include <Implementation/Components/BattleComponent.hpp>
#include <Implementation/Components/StatsComponent.hpp>
#include <gtest/gtest.h>

TEST(WorldUtils, returnTrueWhenFindingPlayer)
{
	ArchetypeManager manager = ArchetypeManager();
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	EXPECT_EQ(true, WorldUtils::getPlayer(manager).has_value());
}

// This is needed when the Battle takes away the inputComponent from the player, so that the system will ignore it
TEST(WorldUtils, returnNullOptWhenPlayerDoesNotHaveComponent)
{
	ArchetypeManager manager = ArchetypeManager();
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	EXPECT_EQ(false, WorldUtils::getPlayersComponent<InputComponent>(manager).has_value());
}
