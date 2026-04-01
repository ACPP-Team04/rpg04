#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/ECS/ECSManager.hpp"
#include "Implementation/Components/InventoryComponent.hpp"
#include <gtest/gtest.h>

TEST(CombatSystemTest, initialFightingSetup)
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

	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemyWeapon);
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
