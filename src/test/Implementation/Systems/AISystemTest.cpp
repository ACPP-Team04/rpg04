#include "Abstract/Combat/Systems/AISystem.hpp"
#include "Abstract/Combat/Components/AIComponent.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/ECS/ECSManager.hpp"

#include <Abstract/Overwordl/Components/InventoryComponent.hpp>
#include <Abstract/Overwordl/Components/ItemHealstatsComponent.hpp>
#include <gtest/gtest.h>
TEST(AISystemTest, executeAILogicHeavyAttack)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem);
	EntityID player = manager.createEntity();
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity();

	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent, AIComponent>(
	    enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	BattleComponent &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	StatsComponent &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	battleComponentE.AP = 2;
	statsComponentE.health = 90;

	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemy);
	enemyWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_A;
	enemyWeapon.weaponType = WeaponType::RANGE;

	aiSystem.executeAILogic(enemy, {player, enemy});
	EXPECT_EQ(BattleState::SELECTED_ACTION, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(BattleAction::HEAVY_ATTACK, manager.getComponent<BattleComponent>(enemy).selectedAction);
}
TEST(AISystemTest, executeAILogicLightAttack)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem);
	EntityID player = manager.createEntity();
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity();

	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent, AIComponent>(
	    enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	BattleComponent &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	StatsComponent &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	battleComponentE.AP = 1;
	statsComponentE.health = 90;

	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemy);
	enemyWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	aiSystem.executeAILogic(enemy, {player, enemy});
	EXPECT_EQ(BattleState::SELECTED_ACTION, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(BattleAction::LIGHT_ATTACK, manager.getComponent<BattleComponent>(enemy).selectedAction);
}
TEST(AISystemTest, executeAILogicHeal)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem);
	EntityID player = manager.createEntity();
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity();

	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent, AIComponent>(
	    enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	BattleComponent &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	StatsComponent &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	InventoryComponent &inventoryComponetE = manager.getComponent<InventoryComponent>(enemy);

	auto healingPack = manager.createEntity<ITEM_HEALSTATS_COMPONENT>();
	inventoryComponetE.addItem(healingPack, ITEM_TYPE::HEALING);
	battleComponentE.AP = 2;
	statsComponentE.health = 19;

	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemy);
	enemyWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	aiSystem.executeAILogic(enemy, {player, enemy});
	EXPECT_EQ(BattleState::SELECTED_ACTION, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(BattleAction::HEAL, manager.getComponent<BattleComponent>(enemy).selectedAction);
}
TEST(AISystemTest, executeAILogicRest)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem);
	EntityID player = manager.createEntity();
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity();

	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent, AIComponent>(
	    enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	BattleComponent &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	StatsComponent &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	battleComponentE.AP = 0;
	statsComponentE.health = 90;
	battleComponentE.numberOfUltimateAttacksUsed = 1;
	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemy);
	enemyWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	aiSystem.executeAILogic(enemy, {player, enemy});
	EXPECT_EQ(BattleState::SELECTED_ACTION, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(BattleAction::REST, manager.getComponent<BattleComponent>(enemy).selectedAction);
}

TEST(AISystemTest, executeAILogicUltimateAttack)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem);
	EntityID player = manager.createEntity();
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity();

	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent, AIComponent>(
	    enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	BattleComponent &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	StatsComponent &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	battleComponentE.AP = 0;
	statsComponentE.health = 90;

	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemy);
	enemyWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	aiSystem.executeAILogic(enemy, {player, enemy});
	EXPECT_EQ(BattleState::SELECTED_ACTION, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(BattleAction::ULTIMATE_ATTACK, manager.getComponent<BattleComponent>(enemy).selectedAction);
}
