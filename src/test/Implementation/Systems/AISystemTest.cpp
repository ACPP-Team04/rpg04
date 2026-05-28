#include "Abstract/Combat/Systems/AISystem.hpp"
#include "Abstract/Combat/Components/AIComponent.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/ECS/ECSManager.hpp"

#include <Abstract/Overwordl/Components/CharacterComponent.hpp>
#include <Abstract/Overwordl/Components/InventoryComponent.hpp>
#include <Abstract/Overwordl/Components/ItemComponent.hpp>
#include <gtest/gtest.h>
TEST(AISystemTest, executeAILogicHeavyAttack)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);
	EntityID player = manager.createEntity<PlayerComponent, CharacterComponent>();
	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity();

	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, AIComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	BattleComponent &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	CharacterComponent &characterComponentE = manager.getComponent<CharacterComponent>(enemy);
	battleComponentE.AP = 2;
	characterComponentE.stats.health = 90;

	BattleComponent &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;

	aiSystem.executeAILogic(enemy, {player, enemy});
	EXPECT_EQ(BattleState::SELECTED_ACTION, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(BattleAction::HEAVY_ATTACK, manager.getComponent<BattleComponent>(enemy).selectedAction);
}
TEST(AISystemTest, executeAILogicLightAttack)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);
	EntityID player = manager.createEntity<PlayerComponent, CharacterComponent>();
	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity();

	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, AIComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	BattleComponent &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	CharacterComponent &characterComponentE = manager.getComponent<CharacterComponent>(enemy);

	battleComponentE.AP = 1;
	characterComponentE.stats.health = 90;
	BattleComponent &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;

	aiSystem.executeAILogic(enemy, {player, enemy});
	EXPECT_EQ(BattleState::SELECTED_ACTION, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(BattleAction::LIGHT_ATTACK, manager.getComponent<BattleComponent>(enemy).selectedAction);
}
TEST(AISystemTest, executeAILogicHeal)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);
	EntityID player = manager.createEntity<PlayerComponent, CharacterComponent>();
	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity();

	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, AIComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	BattleComponent &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	CharacterComponent &characterComponentE = manager.getComponent<CharacterComponent>(enemy);
	BattleComponent &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;

	battleComponentE.AP = 2;
	characterComponentE.stats.health = 19;

	aiSystem.executeAILogic(enemy, {player, enemy});
	EXPECT_EQ(BattleState::SELECTED_ACTION, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(BattleAction::HEAL, manager.getComponent<BattleComponent>(enemy).selectedAction);
}
TEST(AISystemTest, executeAILogicRest)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);
	EntityID player = manager.createEntity<PlayerComponent, CharacterComponent>();
	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity();

	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, AIComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	BattleComponent &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	CharacterComponent &characterE = manager.getComponent<CharacterComponent>(enemy);

	battleComponentE.AP = 0;
	characterE.stats.health = 90;
	battleComponentE.numberOfUltimateAttacksUsed = 1;
	BattleComponent &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;

	aiSystem.executeAILogic(enemy, {player, enemy});
	EXPECT_EQ(BattleState::SELECTED_ACTION, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(BattleAction::REST, manager.getComponent<BattleComponent>(enemy).selectedAction);
}

TEST(AISystemTest, executeAILogicUltimateAttack)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);
	EntityID player = manager.createEntity<PlayerComponent, CharacterComponent>();
	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity();

	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, AIComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	BattleComponent &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	CharacterComponent &characterComponentE = manager.getComponent<CharacterComponent>(enemy);
	battleComponentE.AP = 0;
	characterComponentE.stats.health = 90;
	BattleComponent &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;

	aiSystem.executeAILogic(enemy, {player, enemy});
	EXPECT_EQ(BattleState::SELECTED_ACTION, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(BattleAction::ULTIMATE_ATTACK, manager.getComponent<BattleComponent>(enemy).selectedAction);
}

TEST(AISystemTest, selectTargetWithMultipleEnemies)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);
	EntityID player = manager.createEntity<PlayerComponent, BattleComponent>();
	EntityID enemy = manager.createEntity<BattleComponent>();
	EntityID enemy2 = manager.createEntity<BattleComponent>();
	EntityID battle = manager.createEntity<BattleManagerComponent>();
	BattleComponent &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;

	auto target = aiSystem.selectTarget(enemy, {player, enemy, enemy2});
	EXPECT_EQ(target.has_value(), true);
	EXPECT_EQ(player, target.value());
}
