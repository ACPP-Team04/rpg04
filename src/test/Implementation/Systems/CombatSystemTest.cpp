#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/ECS/ECSManager.hpp"
#include "Implementation/Components/InventoryComponent.hpp"
#include <gtest/gtest.h>

TEST(CombatSystemTest, initialFightingSetupOneRoundLightAttack)
{
	ArchetypeManager manager = ArchetypeManager();
	CombatSystem combatSystem = CombatSystem(manager);
	EntityID player = manager.createEntity();
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity();
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(enemy);
	combatSystem.update();

	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;

	WeaponComponent &playerWeapon = manager.getComponent<WeaponComponent>(player);
	playerWeapon.scalingFactor = ScalingFactor::A;
	playerWeapon.weaponType = WeaponType::MELEE;

	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemy);
	enemyWeapon.scalingFactor = ScalingFactor::B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	combatSystem.update();
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, battleComponentP.battleState);
	EXPECT_EQ(true, battleComponentP.isActiveTurn);
	EXPECT_EQ(false, battleComponentE.isActiveTurn);

	// Simulate BattletInputSystem
	battleComponentP.selectedAction = BattleAction::LIGHT_ATTACK;
	battleComponentP.target = enemy;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, battleComponentP.battleState);
	EXPECT_EQ(1, battleComponentP.AP);
	EXPECT_EQ(92, statsComponentE.health);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, battleComponentP.battleState);
	combatSystem.update();

	EXPECT_EQ(BattleState::NEXT_ROUND, battleComponentP.battleState);

	combatSystem.update();
	EXPECT_EQ(true, battleComponentE.isActiveTurn);
	EXPECT_EQ(BattleState::TURN_START, battleComponentE.battleState);
	EXPECT_EQ(false, battleComponentP.isActiveTurn);
	EXPECT_EQ(1, bmc.currentTurnIndex);
	EXPECT_EQ(false, bmc.isBattleOver);
}

TEST(CombatSystemTest, initialFightingSetupOneRoundHeavyAttack)
{
	ArchetypeManager manager = ArchetypeManager();
	CombatSystem combatSystem = CombatSystem(manager);
	EntityID player = manager.createEntity();
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity();
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(enemy);
	combatSystem.update();

	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;

	WeaponComponent &playerWeapon = manager.getComponent<WeaponComponent>(player);
	playerWeapon.scalingFactor = ScalingFactor::B;
	playerWeapon.weaponType = WeaponType::RANGE;

	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemy);
	enemyWeapon.scalingFactor = ScalingFactor::B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	combatSystem.update();
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, battleComponentP.battleState);
	EXPECT_EQ(true, battleComponentP.isActiveTurn);
	EXPECT_EQ(false, battleComponentE.isActiveTurn);

	// Simulate BattletInputSystem
	battleComponentP.selectedAction = BattleAction::HEAVY_ATTACK;
	battleComponentP.target = enemy;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, battleComponentP.battleState);
	EXPECT_EQ(0, battleComponentP.AP);
	EXPECT_EQ(88, statsComponentE.health);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, battleComponentP.battleState);
	combatSystem.update();

	EXPECT_EQ(BattleState::NEXT_ROUND, battleComponentP.battleState);

	combatSystem.update();
	EXPECT_EQ(true, battleComponentE.isActiveTurn);
	EXPECT_EQ(BattleState::TURN_START, battleComponentE.battleState);
	EXPECT_EQ(false, battleComponentP.isActiveTurn);
	EXPECT_EQ(1, bmc.currentTurnIndex);
	EXPECT_EQ(false, bmc.isBattleOver);
}

TEST(CombatSystemTest, initialFightingSetupOneRoundUltimateAttack)
{
	ArchetypeManager manager = ArchetypeManager();
	CombatSystem combatSystem = CombatSystem(manager);
	EntityID player = manager.createEntity();
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity();
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(enemy);
	combatSystem.update();

	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;

	WeaponComponent &playerWeapon = manager.getComponent<WeaponComponent>(player);
	playerWeapon.scalingFactor = ScalingFactor::C;
	playerWeapon.weaponType = WeaponType::RANGE;

	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemy);
	enemyWeapon.scalingFactor = ScalingFactor::B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	combatSystem.update();
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, battleComponentP.battleState);
	EXPECT_EQ(true, battleComponentP.isActiveTurn);
	EXPECT_EQ(false, battleComponentE.isActiveTurn);

	// Simulate BattletInputSystem
	battleComponentP.selectedAction = BattleAction::ULTIMATE_ATTACK;
	battleComponentP.target = enemy;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, battleComponentP.battleState);
	EXPECT_EQ(2, battleComponentP.AP);
	EXPECT_EQ(84, statsComponentE.health);
	EXPECT_EQ(1, battleComponentP.numberOfUltimateAttacksUsed);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, battleComponentP.battleState);
	combatSystem.update();

	EXPECT_EQ(BattleState::NEXT_ROUND, battleComponentP.battleState);

	combatSystem.update();
	EXPECT_EQ(true, battleComponentE.isActiveTurn);
	EXPECT_EQ(BattleState::TURN_START, battleComponentE.battleState);
	EXPECT_EQ(false, battleComponentP.isActiveTurn);
	EXPECT_EQ(1, bmc.currentTurnIndex);
	EXPECT_EQ(false, bmc.isBattleOver);
}

TEST(CombatSystemTest, initialFightingSetupOneRoundHealWithFullLife)
{
	ArchetypeManager manager = ArchetypeManager();
	CombatSystem combatSystem = CombatSystem(manager);
	EntityID player = manager.createEntity();
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity();
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(enemy);
	combatSystem.update();

	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;

	WeaponComponent &playerWeapon = manager.getComponent<WeaponComponent>(player);
	playerWeapon.scalingFactor = ScalingFactor::C;
	playerWeapon.weaponType = WeaponType::RANGE;

	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemy);
	enemyWeapon.scalingFactor = ScalingFactor::B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	combatSystem.update();
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	auto &inventoryComponentP = manager.getComponent<InventoryComponent>(player);
	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, battleComponentP.battleState);
	EXPECT_EQ(true, battleComponentP.isActiveTurn);
	EXPECT_EQ(false, battleComponentE.isActiveTurn);

	// Simulate BattletInputSystem
	battleComponentP.selectedAction = BattleAction::HEAL;
	battleComponentP.target = enemy;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, battleComponentP.battleState);
	EXPECT_EQ(2, battleComponentP.AP);
	EXPECT_EQ(100, statsComponentE.health);
	EXPECT_EQ(100, statsComponentP.health);
	EXPECT_EQ(0, inventoryComponentP.numberOfHealthPotions);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, battleComponentP.battleState);
	combatSystem.update();

	EXPECT_EQ(BattleState::NEXT_ROUND, battleComponentP.battleState);

	combatSystem.update();
	EXPECT_EQ(true, battleComponentE.isActiveTurn);
	EXPECT_EQ(BattleState::TURN_START, battleComponentE.battleState);
	EXPECT_EQ(false, battleComponentP.isActiveTurn);
	EXPECT_EQ(1, bmc.currentTurnIndex);
	EXPECT_EQ(false, bmc.isBattleOver);
}

TEST(CombatSystemTest, initialFightingSetupOneRoundHealWithNonFullLife)
{
	ArchetypeManager manager = ArchetypeManager();
	CombatSystem combatSystem = CombatSystem(manager);
	EntityID player = manager.createEntity();
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity();
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(enemy);
	combatSystem.update();

	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;

	WeaponComponent &playerWeapon = manager.getComponent<WeaponComponent>(player);
	playerWeapon.scalingFactor = ScalingFactor::C;
	playerWeapon.weaponType = WeaponType::RANGE;

	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemy);
	enemyWeapon.scalingFactor = ScalingFactor::B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	combatSystem.update();
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	statsComponentP.health = 89;
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	auto &inventoryComponentP = manager.getComponent<InventoryComponent>(player);
	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, battleComponentP.battleState);
	EXPECT_EQ(true, battleComponentP.isActiveTurn);
	EXPECT_EQ(false, battleComponentE.isActiveTurn);
	EXPECT_EQ(1, inventoryComponentP.numberOfHealthPotions);
	// Simulate BattletInputSystem
	battleComponentP.selectedAction = BattleAction::HEAL;
	battleComponentP.target = enemy;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, battleComponentP.battleState);
	EXPECT_EQ(2, battleComponentP.AP);
	EXPECT_EQ(100, statsComponentE.health);
	EXPECT_EQ(99, statsComponentP.health);
	EXPECT_EQ(0, inventoryComponentP.numberOfHealthPotions);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, battleComponentP.battleState);
	combatSystem.update();

	EXPECT_EQ(BattleState::NEXT_ROUND, battleComponentP.battleState);

	combatSystem.update();
	EXPECT_EQ(true, battleComponentE.isActiveTurn);
	EXPECT_EQ(BattleState::TURN_START, battleComponentE.battleState);
	EXPECT_EQ(false, battleComponentP.isActiveTurn);
	EXPECT_EQ(1, bmc.currentTurnIndex);
	EXPECT_EQ(false, bmc.isBattleOver);
}

TEST(CombatSystemTest, initialFightingSetupOneRoundRestoreAP)
{
	ArchetypeManager manager = ArchetypeManager();
	CombatSystem combatSystem = CombatSystem(manager);
	EntityID player = manager.createEntity();
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity();
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(enemy);
	combatSystem.update();

	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;

	WeaponComponent &playerWeapon = manager.getComponent<WeaponComponent>(player);
	playerWeapon.scalingFactor = ScalingFactor::C;
	playerWeapon.weaponType = WeaponType::RANGE;

	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemy);
	enemyWeapon.scalingFactor = ScalingFactor::B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	combatSystem.update();
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	auto &inventoryComponentP = manager.getComponent<InventoryComponent>(player);
	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, battleComponentP.battleState);
	EXPECT_EQ(true, battleComponentP.isActiveTurn);
	EXPECT_EQ(false, battleComponentE.isActiveTurn);
	EXPECT_EQ(1, inventoryComponentP.numberOfHealthPotions);
	// Simulate BattletInputSystem
	battleComponentP.selectedAction = BattleAction::REST;
	battleComponentP.target = enemy;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, battleComponentP.battleState);
	EXPECT_EQ(4, battleComponentP.AP);
	EXPECT_EQ(100, statsComponentE.health);
	EXPECT_EQ(100, statsComponentP.health);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, battleComponentP.battleState);
	combatSystem.update();

	EXPECT_EQ(BattleState::NEXT_ROUND, battleComponentP.battleState);

	combatSystem.update();
	EXPECT_EQ(true, battleComponentE.isActiveTurn);
	EXPECT_EQ(BattleState::TURN_START, battleComponentE.battleState);
	EXPECT_EQ(false, battleComponentP.isActiveTurn);
	EXPECT_EQ(1, bmc.currentTurnIndex);
	EXPECT_EQ(false, bmc.isBattleOver);
}

TEST(CombatSystemTest, cleanUpBattlePlayerWon)
{
	ArchetypeManager manager = ArchetypeManager();
	CombatSystem combatSystem = CombatSystem(manager);
	EntityID player = manager.createEntity(EntityTag::PLAYER);
	EntityID enemy = manager.createEntity(EntityTag::ENEMY);
	EntityID battle = manager.createEntity();
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	statsComponentP.health = 50;
	combatSystem.cleanUpBattle(battle, player);

	EXPECT_EQ(100, manager.getComponent<StatsComponent>(player).health);
	EXPECT_EQ(2, manager.getComponent<StatsComponent>(player).experienceLevel);
	EXPECT_EQ(1, manager.getComponent<StatsComponent>(player).numberOfFightsWon);

	EXPECT_EQ(1, manager.getComponent<StatsComponent>(enemy).experienceLevel);
	EXPECT_EQ(0, manager.getComponent<StatsComponent>(enemy).numberOfFightsWon);
}

TEST(CombatSystemTest, cleanUpBattleEnemyWon)
{
	ArchetypeManager manager = ArchetypeManager();
	CombatSystem combatSystem = CombatSystem(manager);
	EntityID player = manager.createEntity(EntityTag::PLAYER);
	EntityID enemy = manager.createEntity(EntityTag::ENEMY);
	EntityID battle = manager.createEntity();
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	statsComponentP.health = 50;
	combatSystem.cleanUpBattle(battle, enemy);

	EXPECT_EQ(100, manager.getComponent<StatsComponent>(enemy).health);
	EXPECT_EQ(1, manager.getComponent<StatsComponent>(enemy).experienceLevel);
	EXPECT_EQ(0, manager.getComponent<StatsComponent>(enemy).numberOfFightsWon);

	EXPECT_EQ(1, manager.getComponent<StatsComponent>(player).experienceLevel);
	EXPECT_EQ(0, manager.getComponent<StatsComponent>(player).numberOfFightsWon);
}

TEST(CombatSystemTest, combatSystemPlayerWon)
{
	ArchetypeManager manager = ArchetypeManager();
	CombatSystem combatSystem = CombatSystem(manager);
	EntityID player = manager.createEntity(EntityTag::PLAYER);
	EntityID enemy = manager.createEntity(EntityTag::ENEMY);
	EntityID battle = manager.createEntity();
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	WeaponComponent &playerWeapon = manager.getComponent<WeaponComponent>(player);
	playerWeapon.scalingFactor = ScalingFactor::C;
	playerWeapon.weaponType = WeaponType::RANGE;

	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	statsComponentE.health = 10;
	battleComponentP.selectedAction = BattleAction::HEAVY_ATTACK;
	battleComponentP.target = enemy;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, battleComponentP.battleState);
	EXPECT_EQ(0, battleComponentP.AP);
	EXPECT_EQ(0, statsComponentE.health);
	EXPECT_EQ(100, statsComponentP.health);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, battleComponentP.battleState);
	combatSystem.update();
	EXPECT_EQ(BattleState::VICTORY, battleComponentP.battleState);
	combatSystem.update();
	combatSystem.update();
	EXPECT_EQ(100, manager.getComponent<StatsComponent>(player).health);
	EXPECT_EQ(2, manager.getComponent<StatsComponent>(player).experienceLevel);
	EXPECT_EQ(1, manager.getComponent<StatsComponent>(player).numberOfFightsWon);

	EXPECT_EQ(0, manager.getComponent<StatsComponent>(enemy).numberOfFightsWon);
}

TEST(CombatSystemTest, combatSystemEnemyWon)
{
	ArchetypeManager manager = ArchetypeManager();
	CombatSystem combatSystem = CombatSystem(manager);
	EntityID player = manager.createEntity(EntityTag::PLAYER);
	EntityID enemy = manager.createEntity(EntityTag::ENEMY);
	EntityID battle = manager.createEntity();
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemy);
	enemyWeapon.scalingFactor = ScalingFactor::C;
	enemyWeapon.weaponType = WeaponType::RANGE;

	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {enemy, player};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	//
	statsComponentP.health = 10;
	battleComponentE.selectedAction = BattleAction::HEAVY_ATTACK;
	battleComponentE.target = player;
	battleComponentE.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, battleComponentE.battleState);
	EXPECT_EQ(0, battleComponentE.AP);
	EXPECT_EQ(0, statsComponentP.health);
	EXPECT_EQ(100, statsComponentE.health);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, battleComponentE.battleState);
	combatSystem.update();
	EXPECT_EQ(BattleState::VICTORY, battleComponentE.battleState);
	combatSystem.update();
	combatSystem.update();

	EXPECT_EQ(100, manager.getComponent<StatsComponent>(enemy).health);
	EXPECT_EQ(1, manager.getComponent<StatsComponent>(player).experienceLevel);
	EXPECT_EQ(0, manager.getComponent<StatsComponent>(enemy).numberOfFightsWon);

	EXPECT_EQ(0, manager.getComponent<StatsComponent>(enemy).numberOfFightsWon);
}
