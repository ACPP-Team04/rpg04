#include "Abstract/Overwordl/SwitchBattleModeSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/StateComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/Overwordl/Components/WorldComponent.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include <gtest/gtest.h>

TEST(SwitchBattleModeTest, ValidatesEnemiesCorrectly)
{
	ArchetypeManager archetypeManager = ArchetypeManager();
	AudioManager audioManager = AudioManager(16, false);
	AudioSystem audioSystem = AudioSystem(archetypeManager, audioManager);
	SwitchBattleModeSystem system(archetypeManager, audioSystem);
	CharacterComponent enemyStats;
	enemyStats.fightable = true;
	enemyStats.stats.health = 100;

	EntityID initialEnemy = EntityID::fromExistingId(99);
	EntityID testEnemy = EntityID::fromExistingId(5);
	std::vector<EntityID> party = {EntityID::fromExistingId(1), EntityID::fromExistingId(2)};

	EXPECT_TRUE(system.isValidAdditionalEnemy(testEnemy, enemyStats, party, initialEnemy));

	EXPECT_FALSE(system.isValidAdditionalEnemy(initialEnemy, enemyStats, party, initialEnemy));

	EXPECT_FALSE(system.isValidAdditionalEnemy(EntityID::fromExistingId(1), enemyStats, party, initialEnemy));

	enemyStats.stats.health = 0;
	EXPECT_FALSE(system.isValidAdditionalEnemy(testEnemy, enemyStats, party, initialEnemy));
}

TEST(SwitchBattleModeTest, DeterminesXpRewardCorrectly)
{
	ArchetypeManager archetypeManager = ArchetypeManager();
	AudioManager audioManager = AudioManager(16, false);
	AudioSystem audioSystem = AudioSystem(archetypeManager, audioManager);
	SwitchBattleModeSystem system(archetypeManager, audioSystem);

	EXPECT_EQ(1, system.determineXpRewardForPlayer(1));
	EXPECT_EQ(3, system.determineXpRewardForPlayer(2));
	EXPECT_EQ(6, system.determineXpRewardForPlayer(3));
}

class SwitchBattleModeIntegrationTest : public ::testing::Test {
  protected:
	ArchetypeManager manager;
	AudioManager dummyAudioManager{16, true};
	AudioSystem dummyAudioSystem{manager, dummyAudioManager};
	SwitchBattleModeSystem switchSystem{manager, dummyAudioSystem};

	EntityID player;
	EntityID enemy;

	void SetUp() override
	{
		EntityID world = manager.createEntity<WorldComponent>();
		auto &worldComp = manager.getComponent<WorldComponent>(world);
		worldComp.currentGroup = 93;
		player = manager.createEntity<PlayerComponent, CharacterComponent, TransformComponent, StateComponent,
		                              InputComponent, PartOfLayerComponent>();
		enemy =
		    manager.createEntity<CharacterComponent, TransformComponent, InteractionComponent, PartOfLayerComponent>();

		manager.getComponent<PartOfLayerComponent>(player).groupId = 93;
		manager.getComponent<PartOfLayerComponent>(enemy).groupId = 93;
		auto &interaction = manager.getComponent<InteractionComponent>(enemy);
		interaction.isActive = true;
		interaction.action = INTERACTION_ACTION::START_BATTLE;
	}
};

TEST_F(SwitchBattleModeIntegrationTest, SuccessfullyTransitionsToBattle)
{
	switchSystem.update();
	EXPECT_FALSE(manager.hasComponent<InputComponent>(player));
	EXPECT_TRUE(manager.hasComponent<BattleComponent>(player));

	bool battleManagerFound = false;
	manager.view<BattleManagerComponent>().each([&](EntityID id, BattleManagerComponent &bmc) {
		battleManagerFound = true;

		EXPECT_EQ(2, bmc.participants.size());
		EXPECT_TRUE(std::ranges::find(bmc.participants, player) != bmc.participants.end());
		EXPECT_TRUE(std::ranges::find(bmc.participants, enemy) != bmc.participants.end());
	});

	EXPECT_TRUE(battleManagerFound);
}