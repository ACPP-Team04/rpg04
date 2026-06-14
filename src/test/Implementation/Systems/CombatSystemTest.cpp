#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Combat/Systems/BattleInputSystem.hpp"
#include "Abstract/ECS/ECSManager.hpp"
#include "Abstract/Overwordl/Components/AnimationComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/StateComponent.hpp"

#include <Abstract/Combat/Components/DeathComponent.hpp>
#include <Abstract/Combat/Components/HitFeedbackComponent.hpp>
#include <Abstract/Combat/Components/LungeComponent.hpp>
#include <Abstract/Overwordl/Components/CharacterComponent.hpp>
#include <Abstract/Overwordl/Components/ItemComponent.hpp>
#include <Abstract/Overwordl/Components/ItemHealstatsComponent.hpp>
#include <Abstract/Overwordl/Components/PartOfLayerComponent.hpp>
#include <Abstract/Overwordl/Components/Player_Component.hpp>
#include <Abstract/Overwordl/Components/TransformComponent.hpp>
#include <gtest/gtest.h>

TEST(CombatSystemTest, initialFightingSetupOneRoundLightAttack)
{

	ArchetypeManager manager = ArchetypeManager();
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, TransformComponent, StateComponent,
	                                       CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;

	AISystem aiSystem = AISystem(manager);
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);
	EntityID enemy = manager.createEntity<TransformComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent, CharacterComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	combatSystem.update();

	auto playerWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &playerWeapon = manager.getComponent<ItemComponent>(playerWeaponId);
	playerWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_A;
	playerWeapon.weaponStats.weaponType = WeaponType::MELEE;
	playerWeapon.weaponStats.lightAttackBaseDmg = 8;

	auto enemyWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &enemyWeapon = manager.getComponent<ItemComponent>(enemyWeaponId);
	enemyWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponStats.weaponType = WeaponType::RANGE;

	CharacterComponent &characterComP = manager.getComponent<CharacterComponent>(player);
	CharacterComponent &characterComE = manager.getComponent<CharacterComponent>(enemy);
	characterComP.equipedWeapon = playerWeaponId.getId();
	characterComE.equipedWeapon = enemyWeaponId.getId();

	combatSystem.update();

	characterComE.stats.health = 92;

	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, battleComponentP.battleState);
	EXPECT_EQ(true, battleComponentP.isActiveTurn);
	EXPECT_EQ(false, battleComponentE.isActiveTurn);

	// Simulate BattletInputSystem
	battleComponentP.selectedAction = BattleAction::LIGHT_ATTACK;
	battleComponentP.target = enemy;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, manager.getComponent<BattleComponent>(player).battleState);
	EXPECT_EQ(1, manager.getComponent<BattleComponent>(player).AP);
	// 92-(8+1*0.5*1)=83.5 -> (int) 83
	EXPECT_EQ(83, characterComE.stats.health);
	manager.removeComponentFromEntity<HitFeedbackComponent>(enemy);
	manager.removeComponentFromEntity<LungeComponent>(player);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, manager.getComponent<BattleComponent>(player).battleState);
	combatSystem.update();

	EXPECT_EQ(BattleState::NEXT_ROUND, manager.getComponent<BattleComponent>(player).battleState);

	combatSystem.update();
	EXPECT_EQ(true, battleComponentE.isActiveTurn);
	EXPECT_EQ(BattleState::TURN_START, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(false, battleComponentP.isActiveTurn);
	EXPECT_EQ(1, bmc.currentTurnIndex);
	EXPECT_EQ(false, bmc.isBattleOver);
}

TEST(CombatSystemTest, initialFightingSetupOneRoundHeavyAttack)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, TransformComponent, StateComponent,
	                                       CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity<TransformComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent, CharacterComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;
	battleComponentE.battleManagerId = battle;
	battleComponentP.battleManagerId = battle;

	combatSystem.update();

	auto playerWeaponId = manager.createEntity<ItemComponent>();
	auto &playerWeapon = manager.getComponent<ItemComponent>(playerWeaponId);
	playerWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	playerWeapon.weaponStats.heavyAttackBaseDmg = 12;
	playerWeapon.weaponStats.scalingStat = STATS::STRENGTH;
	playerWeapon.weaponStats.weaponType = WeaponType::RANGE;

	auto enemyWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &enemyWeapon = manager.getComponent<ItemComponent>(enemyWeaponId);
	enemyWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponStats.weaponType = WeaponType::RANGE;

	CharacterComponent &characterComP = manager.getComponent<CharacterComponent>(player);
	CharacterComponent &characterComE = manager.getComponent<CharacterComponent>(enemy);
	characterComP.equipedWeapon = playerWeaponId.getId();
	characterComE.equipedWeapon = enemyWeaponId.getId();

	combatSystem.update();

	characterComP.stats.addScalableStats(STATS::STRENGTH, 2);
	characterComE.stats.health = 100;

	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, manager.getComponent<BattleComponent>(player).battleState);
	EXPECT_EQ(true, manager.getComponent<BattleComponent>(player).isActiveTurn);
	EXPECT_EQ(false, manager.getComponent<BattleComponent>(enemy).isActiveTurn);

	// Simulate BattletInputSystem
	battleComponentP.selectedAction = BattleAction::HEAVY_ATTACK;
	battleComponentP.target = enemy;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, manager.getComponent<BattleComponent>(player).battleState);
	EXPECT_EQ(0, manager.getComponent<BattleComponent>(player).AP);
	// 100-(12+1*2*0.5*2)=86
	EXPECT_EQ(86, characterComE.stats.health);
	manager.removeComponentFromEntity<HitFeedbackComponent>(enemy);
	manager.removeComponentFromEntity<LungeComponent>(player);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, manager.getComponent<BattleComponent>(player).battleState);
	combatSystem.update();

	EXPECT_EQ(BattleState::NEXT_ROUND, manager.getComponent<BattleComponent>(player).battleState);
	combatSystem.update();
	EXPECT_EQ(true, battleComponentE.isActiveTurn);
	EXPECT_EQ(BattleState::TURN_START, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(false, manager.getComponent<BattleComponent>(player).isActiveTurn);
	EXPECT_EQ(1, bmc.currentTurnIndex);
	EXPECT_EQ(false, bmc.isBattleOver);
}

TEST(CombatSystemTest, initialFightingSetupOneRoundUltimateAttack)
{
	ArchetypeManager manager = ArchetypeManager();
	EntityID world = manager.createEntity<WorldComponent>();

	AISystem aiSystem = AISystem(manager);
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, TransformComponent, StateComponent,
	                                       CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;

	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, CharacterComponent, TransformComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	battleComponentE.battleManagerId = battle;
	battleComponentP.battleManagerId = battle;
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;
	combatSystem.update();

	auto playerWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &playerWeapon = manager.getComponent<ItemComponent>(playerWeaponId);
	playerWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	playerWeapon.weaponStats.weaponType = WeaponType::RANGE;
	playerWeapon.weaponStats.ultimateAttackBaseDmg = 20;

	auto enemyWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &enemyWeapon = manager.getComponent<ItemComponent>(enemyWeaponId);
	enemyWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponStats.weaponType = WeaponType::RANGE;

	CharacterComponent &characterComP = manager.getComponent<CharacterComponent>(player);
	CharacterComponent &characterComE = manager.getComponent<CharacterComponent>(enemy);
	characterComP.equipedWeapon = playerWeaponId.getId();
	characterComE.equipedWeapon = enemyWeaponId.getId();

	combatSystem.update();

	characterComE.stats.health = 100;
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
	EXPECT_EQ(BattleState::EXECUTING_ACTION, manager.getComponent<BattleComponent>(player).battleState);
	EXPECT_EQ(2, manager.getComponent<BattleComponent>(player).AP);
	// 100-(20+1*0.25*3)=79.25 -> (int) 79
	EXPECT_EQ(79, characterComE.stats.health);
	EXPECT_EQ(1, manager.getComponent<BattleComponent>(player).numberOfUltimateAttacksUsed);
	manager.removeComponentFromEntity<HitFeedbackComponent>(enemy);
	manager.removeComponentFromEntity<LungeComponent>(player);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, manager.getComponent<BattleComponent>(player).battleState);
	combatSystem.update();

	EXPECT_EQ(BattleState::NEXT_ROUND, manager.getComponent<BattleComponent>(player).battleState);

	combatSystem.update();
	EXPECT_EQ(true, manager.getComponent<BattleComponent>(enemy).isActiveTurn);
	EXPECT_EQ(BattleState::TURN_START, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(false, manager.getComponent<BattleComponent>(player).isActiveTurn);
	EXPECT_EQ(1, bmc.currentTurnIndex);
	EXPECT_EQ(false, bmc.isBattleOver);
}

TEST(CombatSystemTest, initialFightingSetupOneRoundHealWithFullLife)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	combatSystem.update();

	auto playerWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &playerWeapon = manager.getComponent<ItemComponent>(playerWeaponId);
	playerWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	playerWeapon.weaponStats.weaponType = WeaponType::RANGE;

	auto enemyWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &enemyWeapon = manager.getComponent<ItemComponent>(enemyWeaponId);
	enemyWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponStats.weaponType = WeaponType::RANGE;

	auto &characterCompP = manager.getComponent<CharacterComponent>(player);
	characterCompP.equipedWeapon = playerWeaponId.getId();

	auto &characterCompE = manager.getComponent<CharacterComponent>(enemy);
	characterCompE.equipedWeapon = enemyWeaponId.getId();

	combatSystem.update();

	characterCompP.stats.health = 100;
	characterCompP.stats.addScalableStats(STATS::MAX_HEALTH, 100);

	characterCompE.stats.health = 80;

	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, battleComponentP.battleState);
	EXPECT_EQ(true, battleComponentP.isActiveTurn);
	EXPECT_EQ(false, battleComponentE.isActiveTurn);

	// Simulate BattletInputSystem
	battleComponentP.selectedAction = BattleAction::HEAL;
	battleComponentP.target = player;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, battleComponentP.battleState);
	EXPECT_EQ(0, battleComponentP.AP);
	EXPECT_EQ(80, characterCompE.stats.health);
	EXPECT_EQ(100, characterCompP.stats.health);
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
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	combatSystem.update();

	auto playerWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &playerWeapon = manager.getComponent<ItemComponent>(playerWeaponId);
	playerWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	playerWeapon.weaponStats.weaponType = WeaponType::RANGE;

	auto enemyWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &enemyWeapon = manager.getComponent<ItemComponent>(enemyWeaponId);
	enemyWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponStats.weaponType = WeaponType::RANGE;

	CharacterComponent &characterComP = manager.getComponent<CharacterComponent>(player);
	CharacterComponent &characterComE = manager.getComponent<CharacterComponent>(enemy);
	characterComP.equipedWeapon = playerWeaponId.getId();
	characterComE.equipedWeapon = enemyWeaponId.getId();

	combatSystem.update();

	characterComP.stats.health = 89;
	characterComP.stats.addScalableStats(STATS::MAX_HEALTH, 120);
	characterComP.stats.addScalableStats(STATS::FAITH, 1);

	characterComE.stats.health = 20;

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
	EXPECT_EQ(20, characterComE.stats.health);
	EXPECT_EQ(101, characterComP.stats.health);
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
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);
	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, CharacterComponent>(enemy);

	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;
	combatSystem.update();

	auto playerWeaponId = manager.createEntity<ItemComponent>();

	ItemComponent &playerWeapon = manager.getComponent<ItemComponent>(playerWeaponId);
	playerWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	playerWeapon.weaponStats.weaponType = WeaponType::RANGE;

	auto enemyWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &enemyWeapon = manager.getComponent<ItemComponent>(enemyWeaponId);
	enemyWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponStats.weaponType = WeaponType::RANGE;

	CharacterComponent &characterComP = manager.getComponent<CharacterComponent>(player);
	CharacterComponent &characterComE = manager.getComponent<CharacterComponent>(enemy);
	characterComP.equipedWeapon = playerWeaponId.getId();
	characterComE.equipedWeapon = enemyWeaponId.getId();

	combatSystem.update();
	characterComP.stats.health = 100;
	characterComE.stats.health = 100;

	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, battleComponentP.battleState);
	EXPECT_EQ(true, battleComponentP.isActiveTurn);
	EXPECT_EQ(false, battleComponentE.isActiveTurn);
	// Simulate BattletInputSystem
	battleComponentP.selectedAction = BattleAction::REST;
	battleComponentP.target = enemy;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, battleComponentP.battleState);
	EXPECT_EQ(4, battleComponentP.AP);
	EXPECT_EQ(100, characterComE.stats.health);
	EXPECT_EQ(100, characterComP.stats.health);
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
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, TransformComponent, StateComponent,
	                                       CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity<CharacterComponent, TransformComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	auto &statsComponentP = manager.getComponent<CharacterComponent>(player);
	statsComponentP.stats.addScalableStats(MAX_HEALTH, 110);
	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	statsComponentP.stats.health = 50;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;
	combatSystem.cleanUpBattle(battle, manager.getComponent<BattleComponent>(player).faction, BattleState::VICTORY);

	EXPECT_EQ(110, manager.getComponent<CharacterComponent>(player).stats.health);
	EXPECT_EQ(2, manager.getComponent<CharacterComponent>(player).stats.experienceLevel);
	EXPECT_EQ(1, manager.getComponent<CharacterComponent>(player).stats.numberOfFightsWon);
}

TEST(CombatSystemTest, cleanUpBattleEnemyWon)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	CharacterComponent &statsComponentP = manager.getComponent<CharacterComponent>(player);
	CharacterComponent &statsComponentE = manager.getComponent<CharacterComponent>(enemy);

	statsComponentE.stats.addScalableStats(MAX_HEALTH, 120);
	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	statsComponentP.stats.health = 50;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;
	combatSystem.cleanUpBattle(battle, manager.getComponent<BattleComponent>(enemy).faction, BattleState::DEFEAT);

	EXPECT_EQ(120, manager.getComponent<CharacterComponent>(enemy).stats.health);
	EXPECT_EQ(2, manager.getComponent<CharacterComponent>(enemy).stats.experienceLevel);
	EXPECT_EQ(1, manager.getComponent<CharacterComponent>(enemy).stats.numberOfFightsWon);

	EXPECT_EQ(1, manager.getComponent<CharacterComponent>(player).stats.experienceLevel);
	EXPECT_EQ(0, manager.getComponent<CharacterComponent>(player).stats.numberOfFightsWon);
}

TEST(CombatSystemTest, combatSystemPlayerWon)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, TransformComponent, StateComponent,
	                                       CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);
	EntityID enemy = manager.createEntity<CharacterComponent, TransformComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &characterComponentP = manager.getComponent<CharacterComponent>(player);
	characterComponentP.stats.addScalableStats(MAX_HEALTH, 110);
	characterComponentP.stats.health = 100;
	auto &characterComponentE = manager.getComponent<CharacterComponent>(enemy);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);

	auto playerWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &playerWeapon = manager.getComponent<ItemComponent>(playerWeaponId);
	playerWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	playerWeapon.weaponStats.weaponType = WeaponType::RANGE;
	playerWeapon.weaponStats.heavyAttackBaseDmg = 30;

	characterComponentP.equipedWeapon = playerWeaponId.getId();

	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	characterComponentE.stats.health = 10;
	battleComponentP.selectedAction = BattleAction::HEAVY_ATTACK;
	battleComponentP.target = enemy;
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, manager.getComponent<BattleComponent>(player).battleState);
	EXPECT_EQ(0, manager.getComponent<BattleComponent>(player).AP);
	EXPECT_EQ(0, manager.getComponent<CharacterComponent>(enemy).stats.health);
	EXPECT_EQ(100, manager.getComponent<CharacterComponent>(player).stats.health);
	manager.removeComponentFromEntity<HitFeedbackComponent>(enemy);
	manager.removeComponentFromEntity<LungeComponent>(player);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, manager.getComponent<BattleComponent>(player).battleState);
	combatSystem.update();
	EXPECT_EQ(BattleState::STATS_DISTRIBUTION, manager.getComponent<BattleComponent>(player).battleState);
	// Simulating stats distribution imput
	battleComponentP.battleState = BattleState::VICTORY;
	//
	combatSystem.update();
	combatSystem.update();
	EXPECT_EQ(110, manager.getComponent<CharacterComponent>(player).stats.health);
	EXPECT_EQ(2, manager.getComponent<CharacterComponent>(player).stats.experienceLevel);
	EXPECT_EQ(1, manager.getComponent<CharacterComponent>(player).stats.numberOfFightsWon);
}

TEST(CombatSystemTest, combatSystemEnemyWon)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, TransformComponent, StateComponent,
	                                       CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity<CharacterComponent, TransformComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &statsComponentP = manager.getComponent<CharacterComponent>(player);
	auto &statsComponentE = manager.getComponent<CharacterComponent>(enemy);
	statsComponentE.stats.health = 100;
	statsComponentE.stats.addScalableStats(MAX_HEALTH, 120);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;

	auto enemyWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &enemyWeapon = manager.getComponent<ItemComponent>(enemyWeaponId);

	enemyWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	enemyWeapon.weaponStats.weaponType = WeaponType::RANGE;
	enemyWeapon.weaponStats.heavyAttackBaseDmg = 30;

	statsComponentE.equipedWeapon = enemyWeaponId.getId();

	auto &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {enemy, player};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	//
	statsComponentP.stats.health = 10;
	battleComponentE.selectedAction = BattleAction::HEAVY_ATTACK;
	battleComponentE.target = player;
	battleComponentE.battleState = BattleState::SELECTED_ACTION;
	//

	combatSystem.update();
	EXPECT_EQ(BattleState::EXECUTING_ACTION, manager.getComponent<BattleComponent>(enemy).battleState);
	EXPECT_EQ(0, manager.getComponent<BattleComponent>(enemy).AP);
	EXPECT_EQ(0, manager.getComponent<CharacterComponent>(player).stats.health);
	EXPECT_EQ(100, manager.getComponent<CharacterComponent>(enemy).stats.health);
	manager.removeComponentFromEntity<HitFeedbackComponent>(player);
	manager.removeComponentFromEntity<LungeComponent>(enemy);
	combatSystem.update();
	EXPECT_EQ(BattleState::CHECK_DEATH, manager.getComponent<BattleComponent>(enemy).battleState);
	combatSystem.update();
	EXPECT_EQ(BattleState::DEFEAT, manager.getComponent<BattleComponent>(enemy).battleState);
	combatSystem.update();
	combatSystem.update();

	EXPECT_EQ(120, manager.getComponent<CharacterComponent>(enemy).stats.health);
	EXPECT_EQ(1, manager.getComponent<CharacterComponent>(player).stats.experienceLevel);
	EXPECT_EQ(1, manager.getComponent<CharacterComponent>(enemy).stats.numberOfFightsWon);

	EXPECT_EQ(0, manager.getComponent<CharacterComponent>(player).stats.numberOfFightsWon);
}

TEST(CombatSystemTest, checkDeathConditionPlayerLost)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID enemy2 = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	CharacterComponent &playerCharacter = manager.getComponent<CharacterComponent>(player);
	playerCharacter.stats.health = 0;

	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &battleComponentE2 = manager.getComponent<BattleComponent>(enemy2);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentE2.battleManagerId = battle;
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;

	auto returnState = combatSystem.checkDeathCondition(player, enemy);
	EXPECT_EQ(BattleState::DEFEAT, returnState);
}

TEST(CombatSystemTest, checkDeathConditionPlayerWon)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID enemy2 = manager.createEntity<CharacterComponent, StateComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	CharacterComponent &statsComponentP = manager.getComponent<CharacterComponent>(player);
	CharacterComponent &statsComponentE2 = manager.getComponent<CharacterComponent>(enemy2);
	statsComponentP.stats.health = 10;
	statsComponentE2.stats.health = 0;

	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	auto &battleComponentE2 = manager.getComponent<BattleComponent>(enemy2);
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentE2.battleManagerId = battle;
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;

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
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID enemy2 = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	CharacterComponent &playerCharacter = manager.getComponent<CharacterComponent>(player);
	CharacterComponent &enemyCharacter = manager.getComponent<CharacterComponent>(enemy);
	playerCharacter.stats.health = 10;
	enemyCharacter.stats.health = 20;

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
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;
	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID enemy2 = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &characterComponentP = manager.getComponent<CharacterComponent>(player);
	auto &characterComponentE2 = manager.getComponent<CharacterComponent>(enemy2);
	characterComponentP.stats.health = 10;
	characterComponentE2.stats.health = 20;

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
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;

	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID enemy2 = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &characterCompP = manager.getComponent<CharacterComponent>(player);
	auto &characterCompE2 = manager.getComponent<CharacterComponent>(enemy2);
	characterCompP.stats.health = 10;
	characterCompE2.stats.health = 20;

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
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity<CharacterComponent>();
	EntityID enemy2 = manager.createEntity<CharacterComponent>();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	auto &characterCompP = manager.getComponent<CharacterComponent>(player);
	auto &characterCompE2 = manager.getComponent<CharacterComponent>(enemy2);
	characterCompP.stats.health = 10;
	characterCompE2.stats.health = 20;

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
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity();
	EntityID enemy2 = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, InventoryComponent, StateComponent>(enemy);
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
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager(16, true);
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity();
	EntityID enemy2 = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);

	manager.addComponentToEntity<BattleComponent, TransformComponent>(player);
	manager.addComponentToEntity<BattleComponent, CharacterComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleComponent, CharacterComponent, StateComponent>(enemy2);
	manager.addComponentToEntity<BattleManagerComponent>(battle);

	CharacterComponent &characterComP = manager.getComponent<CharacterComponent>(player);
	CharacterComponent &characterComE2 = manager.getComponent<CharacterComponent>(enemy2);
	characterComP.stats.health = 10;
	characterComE2.stats.health = 20;

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

TEST(CombatSystemTest, initialFightingSetupOneRoundHealWithoutSettingTarget)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent, StatsComponent>(player);
	manager.addComponentToEntity<BattleComponent, StatsComponent, CharacterComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	combatSystem.update();

	auto playerWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &playerWeapon = manager.getComponent<ItemComponent>(playerWeaponId);
	playerWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	playerWeapon.weaponStats.weaponType = WeaponType::RANGE;

	auto enemyWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &enemyWeapon = manager.getComponent<ItemComponent>(enemyWeaponId);
	enemyWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponStats.weaponType = WeaponType::RANGE;

	CharacterComponent &characterComP = manager.getComponent<CharacterComponent>(player);
	CharacterComponent &characterComE = manager.getComponent<CharacterComponent>(enemy);
	characterComP.equipedWeapon = playerWeaponId.getId();
	characterComE.equipedWeapon = enemyWeaponId.getId();

	combatSystem.update();

	characterComP.stats.health = 100;
	characterComP.stats.addScalableStats(STATS::MAX_HEALTH, 100);
	characterComE.stats.health = 80;
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, battleComponentP.battleState);
	EXPECT_EQ(true, battleComponentP.isActiveTurn);
	EXPECT_EQ(false, battleComponentE.isActiveTurn);

	// Simulate BattletInputSystem
	battleComponentP.selectedAction = BattleAction::HEAL;
	// No target set
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	EXPECT_THROW(combatSystem.update(), std::runtime_error);
}

TEST(CombatSystemTest, initialFightingSetupOneRoundRestWithoutSettingTarget)
{
	ArchetypeManager manager = ArchetypeManager();
	AISystem aiSystem = AISystem(manager);
	EntityID world = manager.createEntity<WorldComponent>();
	EntityID player = manager.createEntity<PlayerComponent, PartOfLayerComponent, StateComponent, CharacterComponent>();
	auto &playerLayer = manager.getComponent<PartOfLayerComponent>(player);
	auto &worldLayer = manager.getComponent<WorldComponent>(world);

	worldLayer.currentLayer = LAYERTYPE::OVERWORLD;
	worldLayer.currentLevel = LEVEL_NAME::LEVEL1;

	worldLayer.currentGroup = 93;

	playerLayer.groupId = 93;
	AudioManager audioManager = AudioManager();
	AudioSystem audiosystem = AudioSystem(manager, audioManager);
	CombatSystem combatSystem = CombatSystem(manager, aiSystem, audiosystem);

	EntityID enemy = manager.createEntity();
	EntityID battle = manager.createEntity(EntityTag::BATTLEMANAGER);
	manager.addComponentToEntity<BattleComponent>(player);
	manager.addComponentToEntity<BattleComponent, CharacterComponent, StateComponent>(enemy);
	manager.addComponentToEntity<BattleManagerComponent>(battle);
	BattleManagerComponent &bmc = manager.getComponent<BattleManagerComponent>(battle);
	bmc.participants = {player, enemy};
	bmc.currentTurnIndex = 0;
	bmc.isBattleOver = false;
	auto &battleComponentP = manager.getComponent<BattleComponent>(player);
	auto &battleComponentE = manager.getComponent<BattleComponent>(enemy);
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;
	combatSystem.update();

	auto playerWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &playerWeapon = manager.getComponent<ItemComponent>(playerWeaponId);
	playerWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_C;
	playerWeapon.weaponStats.weaponType = WeaponType::RANGE;

	auto enemyWeaponId = manager.createEntity<ItemComponent>();
	ItemComponent &enemyWeapon = manager.getComponent<ItemComponent>(enemyWeaponId);
	enemyWeapon.weaponStats.scalingFactor = WEAPON_SCALING_FACTOR::SCALE_B;
	enemyWeapon.weaponStats.weaponType = WeaponType::RANGE;

	CharacterComponent &characterComP = manager.getComponent<CharacterComponent>(player);
	CharacterComponent &characterComE = manager.getComponent<CharacterComponent>(enemy);
	characterComP.equipedWeapon = playerWeaponId.getId();
	characterComE.equipedWeapon = enemyWeaponId.getId();

	combatSystem.update();

	characterComP.stats.health = 100;
	characterComP.stats.addScalableStats(STATS::MAX_HEALTH, 100);

	characterComE.stats.health = 80;
	battleComponentP.battleManagerId = battle;
	battleComponentE.battleManagerId = battle;
	battleComponentP.faction = BATTLE_FACTION::PLAYER_PARTY;
	battleComponentP.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;
	EXPECT_EQ(BattleState::WAITING_FOR_INPUT, battleComponentP.battleState);
	EXPECT_EQ(true, battleComponentP.isActiveTurn);
	EXPECT_EQ(false, battleComponentE.isActiveTurn);

	// Simulate BattletInputSystem
	battleComponentP.selectedAction = BattleAction::REST;
	// No target set
	battleComponentP.battleState = BattleState::SELECTED_ACTION;
	//

	EXPECT_THROW(combatSystem.update(), std::runtime_error);
}