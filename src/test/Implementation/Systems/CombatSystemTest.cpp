#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/ECS/ECSManager.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include <Abstract/Combat/Components/DeathComponent.hpp>
#include <Abstract/Overwordl/Components/ItemHealstatsComponent.hpp>
#include <Abstract/Overwordl/Components/PartOfLayerComponent.hpp>
#include <Abstract/Overwordl/Components/Player_Component.hpp>
#include <Abstract/Overwordl/Components/TransformComponent.hpp>
#include <gtest/gtest.h>

TEST(CombatSystemTest, initialFightingSetupOneRoundLightAttack)
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

	AISystem aiSystem = AISystem(manager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem);
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	combatSystem.update();

	auto playerWeaponId = manager.createEntity<WeaponComponent>();
	auto &playerWeapon = manager.getComponent<WeaponComponent>(playerWeaponId);
	playerWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_A;
	playerWeapon.weaponType = WeaponType::MELEE;
	playerWeapon.lightAttackBaseDmg = 8;

	auto &playerInventory = manager.getComponent<InventoryComponent>(player);
	playerInventory.addItem(playerWeaponId, ITEM_TYPE::WEAPON);
	playerInventory.equip(playerWeaponId, ITEM_TYPE::WEAPON);

	auto enemyWeaponId = manager.createEntity<WeaponComponent>();
	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemyWeaponId);
	enemyWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	auto &enemyInventory = manager.getComponent<InventoryComponent>(enemy);
	enemyInventory.addItem(enemyWeaponId, ITEM_TYPE::WEAPON);
	enemyInventory.equip(enemyWeaponId, ITEM_TYPE::WEAPON);

	combatSystem.update();

	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	statsComponentE.health = 92;

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
	// 92-(8+1*0.5*1)=83.5 -> (int) 83
	EXPECT_EQ(83, statsComponentE.health);
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
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	battleComponentE.battleManagerId = battle;
	battleComponentP.battleManagerId = battle;

	combatSystem.update();

	auto playerWeaponId = manager.createEntity<WeaponComponent>();
	auto &playerWeapon = manager.getComponent<WeaponComponent>(playerWeaponId);
	playerWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	playerWeapon.heavyAttackBaseDmg = 12;
	playerWeapon.scalingStat = STATS::STRENGTH;
	playerWeapon.weaponType = WeaponType::RANGE;

	auto &playerInventory = manager.getComponent<InventoryComponent>(player);
	playerInventory.addItem(playerWeaponId, ITEM_TYPE::WEAPON);
	playerInventory.equip(playerWeaponId, ITEM_TYPE::WEAPON);

	auto enemyWeaponId = manager.createEntity<WeaponComponent>();
	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemyWeaponId);
	enemyWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	auto &enemyInventory = manager.getComponent<InventoryComponent>(enemy);
	enemyInventory.addItem(enemyWeaponId, ITEM_TYPE::WEAPON);
	enemyInventory.equip(enemyWeaponId, ITEM_TYPE::WEAPON);

	combatSystem.update();
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	statsComponentP.stats[STATS::STRENGTH] = 2;
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	statsComponentE.health = 100;

	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
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
	// 100-(12+1*2*0.5*2)=86
	EXPECT_EQ(86, statsComponentE.health);
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
	EntityID world = manager.createEntity<WorldComponent>();

	AISystem aiSystem = AISystem(manager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem);
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	battleComponentE.battleManagerId = battle;
	battleComponentP.battleManagerId = battle;

	combatSystem.update();

	auto playerWeaponId = manager.createEntity<WeaponComponent>();
	WeaponComponent &playerWeapon = manager.getComponent<WeaponComponent>(playerWeaponId);
	playerWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	playerWeapon.weaponType = WeaponType::RANGE;
	playerWeapon.ultimateAttackBaseDmg = 20;

	auto enemyWeaponId = manager.createEntity<WeaponComponent>();
	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemyWeaponId);
	enemyWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	auto &playerInventory = manager.getComponent<InventoryComponent>(player);
	playerInventory.addItem(playerWeaponId, ITEM_TYPE::WEAPON);
	playerInventory.equip(playerWeaponId, ITEM_TYPE::WEAPON);

	auto &enemyInventory = manager.getComponent<InventoryComponent>(enemy);
	enemyInventory.addItem(enemyWeaponId, ITEM_TYPE::WEAPON);
	enemyInventory.equip(enemyWeaponId, ITEM_TYPE::WEAPON);

	combatSystem.update();

	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	statsComponentE.health = 100;
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
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
	// 100-(20+1*0.25*3)=79.25 -> (int) 79
	EXPECT_EQ(79, statsComponentE.health);
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
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	combatSystem.update();

	auto playerWeaponId = manager.createEntity<WeaponComponent>();
	WeaponComponent &playerWeapon = manager.getComponent<WeaponComponent>(playerWeaponId);
	playerWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	playerWeapon.weaponType = WeaponType::RANGE;

	auto enemyWeaponId = manager.createEntity<WeaponComponent>();
	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemyWeaponId);
	enemyWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	auto &playerInventory = manager.getComponent<InventoryComponent>(player);
	playerInventory.addItem(playerWeaponId, ITEM_TYPE::WEAPON);
	playerInventory.equip(playerWeaponId, ITEM_TYPE::WEAPON);

	auto &enemyInventory = manager.getComponent<InventoryComponent>(enemy);
	enemyInventory.addItem(enemyWeaponId, ITEM_TYPE::WEAPON);
	enemyInventory.equip(enemyWeaponId, ITEM_TYPE::WEAPON);

	combatSystem.update();

	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	statsComponentP.health = 100;
	statsComponentP.stats[STATS::MAX_HEALTH] = 100;
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	statsComponentE.health = 80;
	auto &inventoryComponentP = manager.getComponent<InventoryComponent>(player);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
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
	EXPECT_EQ(0, battleComponentP.AP);
	EXPECT_EQ(80, statsComponentE.health);
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

TEST(CombatSystemTest, initialFightingSetupOneRoundHealWithNonFullLife)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	combatSystem.update();

	auto playerWeaponId = manager.createEntity<WeaponComponent>();
	WeaponComponent &playerWeapon = manager.getComponent<WeaponComponent>(playerWeaponId);
	playerWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	playerWeapon.weaponType = WeaponType::RANGE;

	auto enemyWeaponId = manager.createEntity<WeaponComponent>();
	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemyWeaponId);
	enemyWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	auto &playerInventory = manager.getComponent<InventoryComponent>(player);
	playerInventory.addItem(playerWeaponId, ITEM_TYPE::WEAPON);
	playerInventory.equip(playerWeaponId, ITEM_TYPE::WEAPON);

	auto &enemyInventory = manager.getComponent<InventoryComponent>(enemy);
	enemyInventory.addItem(enemyWeaponId, ITEM_TYPE::WEAPON);
	enemyInventory.equip(enemyWeaponId, ITEM_TYPE::WEAPON);
	combatSystem.update();
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	statsComponentP.health = 89;
	statsComponentP.stats[STATS::MAX_HEALTH] = 120;
	statsComponentP.stats[STATS::FAITH] = 1;

	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	statsComponentE.health = 20;
	auto &inventoryComponentP = manager.getComponent<InventoryComponent>(player);
	auto healingPack = manager.createEntity<ITEM_HEALSTATS_COMPONENT>();
	inventoryComponentP.addItem(healingPack, ITEM_TYPE::HEALING);

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
	EXPECT_EQ(0, battleComponentP.AP);
	EXPECT_EQ(20, statsComponentE.health);
	EXPECT_EQ(101, statsComponentP.health);
	EXPECT_EQ(1, battleComponentP.numberOfHealsUsed);

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
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, WeaponComponent, InventoryComponent>(enemy);

	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	combatSystem.update();

	auto playerWeaponId = manager.createEntity<WeaponComponent>();

	WeaponComponent &playerWeapon = manager.getComponent<WeaponComponent>(playerWeaponId);
	playerWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	playerWeapon.weaponType = WeaponType::RANGE;

	auto enemyWeaponId = manager.createEntity<WeaponComponent>();
	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemyWeaponId);
	enemyWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponType = WeaponType::RANGE;

	auto &playerInventory = manager.getComponent<InventoryComponent>(player);
	playerInventory.addItem(playerWeaponId, ITEM_TYPE::WEAPON);
	playerInventory.equip(playerWeaponId, ITEM_TYPE::WEAPON);

	auto &enemyInventory = manager.getComponent<InventoryComponent>(enemy);

	enemyInventory.addItem(enemyWeaponId, ITEM_TYPE::WEAPON);
	enemyInventory.equip(enemyWeaponId, ITEM_TYPE::WEAPON);

	combatSystem.update();
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	statsComponentP.health = 100;
	statsComponentE.health = 100;
	auto &inventoryComponentP = manager.getComponent<InventoryComponent>(player);

	auto healingPack = manager.createEntity<ITEM_HEALSTATS_COMPONENT>();
	inventoryComponentP.addItem(healingPack, ITEM_TYPE::HEALING);
	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, battleComponentP.battleState);
	EXPECT_EQ(true, battleComponentP.isActiveTurn);
	EXPECT_EQ(false, battleComponentE.isActiveTurn);
	EXPECT_EQ(1, inventoryComponentP.getItems(ITEM_TYPE::HEALING).size());
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
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	statsComponentP.stats[MAX_HEALTH] = 110;
	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	statsComponentP.health = 50;
	combatSystem.cleanUpBattle(battle, player, BattleState::VICTORY);

	EXPECT_EQ(110, manager.getComponent<StatsComponent>(player).health);
	EXPECT_EQ(2, manager.getComponent<StatsComponent>(player).experienceLevel);
	EXPECT_EQ(1, manager.getComponent<StatsComponent>(player).numberOfFightsWon);
}

TEST(CombatSystemTest, cleanUpBattleEnemyWon)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	statsComponentE.stats[MAX_HEALTH] = 120;
	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	statsComponentP.health = 50;
	combatSystem.cleanUpBattle(battle, enemy, BattleState::DEFEAT);

	EXPECT_EQ(120, manager.getComponent<StatsComponent>(enemy).health);
	EXPECT_EQ(1, manager.getComponent<StatsComponent>(enemy).experienceLevel);
	EXPECT_EQ(0, manager.getComponent<StatsComponent>(enemy).numberOfFightsWon);

	EXPECT_EQ(1, manager.getComponent<StatsComponent>(player).experienceLevel);
	EXPECT_EQ(0, manager.getComponent<StatsComponent>(player).numberOfFightsWon);
}

TEST(CombatSystemTest, combatSystemPlayerWon)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	statsComponentP.stats[MAX_HEALTH] = 110;
	statsComponentP.health = 100;
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto playerWeaponId = manager.createEntity<WeaponComponent>();
	WeaponComponent &playerWeapon = manager.getComponent<WeaponComponent>(playerWeaponId);
	playerWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	playerWeapon.weaponType = WeaponType::RANGE;
	playerWeapon.heavyAttackBaseDmg = 30;
	auto &playerInventory = manager.getComponent<InventoryComponent>(player);
	playerInventory.addItem(playerWeaponId, ITEM_TYPE::WEAPON);
	playerInventory.equip(playerWeaponId, ITEM_TYPE::WEAPON);

	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	statsComponentE.health = 10;
	battleComponentP.selectedAction = BattleAction::HEAVY_ATTACK;
	battleComponentP.target = enemy;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, battleComponentP.battleState);
	EXPECT_EQ(0, battleComponentP.AP);
	EXPECT_EQ(0, statsComponentE.health);
	EXPECT_EQ(100, statsComponentP.health);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, battleComponentP.battleState);
	combatSystem.update();
	EXPECT_EQ(BattleState::STATS_DISTRIBUTION, battleComponentP.battleState);
	// Simulating stats distribution imput
	battleComponentP.battleState = BattleState::VICTORY;
	//
	combatSystem.update();
	combatSystem.update();
	EXPECT_EQ(110, manager.getComponent<StatsComponent>(player).health);
	EXPECT_EQ(2, manager.getComponent<StatsComponent>(player).experienceLevel);
	EXPECT_EQ(1, manager.getComponent<StatsComponent>(player).numberOfFightsWon);
}

TEST(CombatSystemTest, combatSystemEnemyWon)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	statsComponentE.health = 100;
	statsComponentE.stats[MAX_HEALTH] = 120;
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	auto enemyWeaponId = manager.createEntity<WeaponComponent>();
	WeaponComponent &enemyWeapon = manager.getComponent<WeaponComponent>(enemyWeaponId);
	enemyWeapon.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	enemyWeapon.weaponType = WeaponType::RANGE;
	enemyWeapon.heavyAttackBaseDmg = 30;

	auto &enemyInventory = manager.getComponent<InventoryComponent>(enemy);
	enemyInventory.addItem(enemyWeaponId, ITEM_TYPE::WEAPON);
	enemyInventory.equip(enemyWeaponId, ITEM_TYPE::WEAPON);

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
	EXPECT_EQ(BattleState::DEFEAT, battleComponentE.battleState);
	combatSystem.update();
	combatSystem.update();

	EXPECT_EQ(120, manager.getComponent<StatsComponent>(enemy).health);
	EXPECT_EQ(1, manager.getComponent<StatsComponent>(player).experienceLevel);
	EXPECT_EQ(0, manager.getComponent<StatsComponent>(enemy).numberOfFightsWon);

	EXPECT_EQ(0, manager.getComponent<StatsComponent>(player).numberOfFightsWon);
}

TEST(CombatSystemTest, checkDeathConditionPlayerLost)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID enemy2 = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE = manager.getComponent<StatsComponent>(enemy);
	statsComponentP.health = 0;

	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &battleComponentE2 = manager.getComponent<BattleComponent>(enemy2);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentE2.battleManagerId = battle;

	auto returnState = combatSystem.checkDeathCondition(player, enemy);
	EXPECT_EQ(BattleState::DEFEAT, returnState);
}

TEST(CombatSystemTest, checkDeathConditionPlayerWon)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID enemy2 = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE2 = manager.getComponent<StatsComponent>(enemy2);
	statsComponentP.health = 10;
	statsComponentE2.health = 0;

	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &battleComponentE2 = manager.getComponent<BattleComponent>(enemy2);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentE2.battleManagerId = battle;

	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy, enemy2};
	manager.addComponentToEntity<DeathComponent>(enemy);
	auto returnState = combatSystem.checkDeathCondition(enemy2, player);
	EXPECT_EQ(BattleState::VICTORY, returnState);
}

TEST(CombatSystemTest, checkDeathConditionNextRound)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID enemy2 = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE2 = manager.getComponent<StatsComponent>(enemy2);
	statsComponentP.health = 10;
	statsComponentE2.health = 20;

	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &battleComponentE2 = manager.getComponent<BattleComponent>(enemy2);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentE2.battleManagerId = battle;

	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy, enemy2};
	manager.addComponentToEntity<DeathComponent>(enemy);
	auto returnState = combatSystem.checkDeathCondition(enemy2, player);
	EXPECT_EQ(BattleState::NEXT_ROUND, returnState);
}

TEST(CombatSystemTest, getAttackerBaseCase)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID enemy2 = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE2 = manager.getComponent<StatsComponent>(enemy2);
	statsComponentP.health = 10;
	statsComponentE2.health = 20;

	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &battleComponentE2 = manager.getComponent<BattleComponent>(enemy2);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentE2.battleManagerId = battle;

	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy, enemy2};
	bmc.currentTurnIndex = 1;
	manager.addComponentToEntity<DeathComponent>(enemy2);
	auto nextTurnID = combatSystem.getAttacker(bmc);
	EXPECT_EQ(enemy, nextTurnID);
	EXPECT_EQ(1, bmc.currentTurnIndex);
}

TEST(CombatSystemTest, getAttackerSkipDeathEnemy)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID enemy2 = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE2 = manager.getComponent<StatsComponent>(enemy2);
	statsComponentP.health = 10;
	statsComponentE2.health = 20;

	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &battleComponentE2 = manager.getComponent<BattleComponent>(enemy2);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentE2.battleManagerId = battle;

	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy, enemy2};
	bmc.currentTurnIndex = 1;
	manager.addComponentToEntity<DeathComponent>(enemy);
	auto nextTurnID = combatSystem.getAttacker(bmc);
	EXPECT_EQ(enemy2, nextTurnID);
	EXPECT_EQ(2, bmc.currentTurnIndex);
}

TEST(CombatSystemTest, getAttackerCheckBoundaryCondition)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID enemy2 = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE2 = manager.getComponent<StatsComponent>(enemy2);
	statsComponentP.health = 10;
	statsComponentE2.health = 20;

	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &battleComponentE2 = manager.getComponent<BattleComponent>(enemy2);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentE2.battleManagerId = battle;

	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy, enemy2};
	bmc.currentTurnIndex = 1;
	manager.addComponentToEntity<DeathComponent>(enemy);
	manager.addComponentToEntity<DeathComponent>(enemy2);
	manager.addComponentToEntity<DeathComponent>(player);
	EXPECT_THROW(combatSystem.getAttacker(bmc), std::runtime_error);
}

TEST(CombatSystemTest, passTurnAllActiveParticipants)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID enemy2 = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE2 = manager.getComponent<StatsComponent>(enemy2);
	statsComponentP.health = 10;
	statsComponentE2.health = 20;

	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &battleComponentE2 = manager.getComponent<BattleComponent>(enemy2);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentE2.battleManagerId = battle;

	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy, enemy2};
	bmc.currentTurnIndex = 1;
	combatSystem.passTurn(enemy, bmc);
	EXPECT_EQ(2, bmc.currentTurnIndex);
	EXPECT_EQ(false, manager.getComponent<BattleComponent>(enemy).isActiveTurn);
	EXPECT_EQ(true, manager.getComponent<BattleComponent>(enemy2).isActiveTurn);
	EXPECT_EQ(BattleState::TURN_START, manager.getComponent<BattleComponent>(enemy2).battleState);
}

TEST(CombatSystemTest, passTurnOneDeathParticipant)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	playerLayer.layer = LAYERTYPE::OVERWORLD;
	playerLayer.level = LEVEL_NAME::LEVEL1;

	CombatSystem combatSystem = CombatSystem(manager, aiSystem);

	EntityID enemy = manager.createEntity();
	EntityID enemy2 = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<StatsComponent>(player);
	auto &statsComponentE2 = manager.getComponent<StatsComponent>(enemy2);
	statsComponentP.health = 10;
	statsComponentE2.health = 20;

	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &battleComponentE2 = manager.getComponent<BattleComponent>(enemy2);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentE2.battleManagerId = battle;

	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);

	manager.addComponentToEntity<DeathComponent>(enemy2);

	bmc.participants = {player, enemy, enemy2};
	bmc.currentTurnIndex = 1;
	combatSystem.passTurn(enemy, bmc);
	EXPECT_EQ(3, bmc.currentTurnIndex);
	EXPECT_EQ(false, manager.getComponent<BattleComponent>(enemy).isActiveTurn);
	EXPECT_EQ(false, manager.getComponent<BattleComponent>(enemy2).isActiveTurn);
	EXPECT_EQ(true, manager.getComponent<BattleComponent>(player).isActiveTurn);
	EXPECT_EQ(BattleState::TURN_START, manager.getComponent<BattleComponent>(player).battleState);
}
