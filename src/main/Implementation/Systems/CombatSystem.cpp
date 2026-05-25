#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Combat/Components/CombatGodMode.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemHealstatsComponent.hpp"
#include "Abstract/Overwordl/Components/StateComponent.hpp"
#include "Abstract/Utils/GraveConfig.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/StatsComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
#include <Abstract/Combat/Components/DeathComponent.hpp>
#include <Abstract/Combat/Systems/BattleInputSystem.hpp>
#include <Abstract/Overwordl/Components/InputComponent.hpp>
#include <Abstract/Overwordl/Components/MovementComponent.hpp>
#include <Abstract/Overwordl/Components/RenderComponent.hpp>
#include <Abstract/Overwordl/Components/TransformComponent.hpp>
#include <Abstract/TILE_ENUMS.hpp>
#include <Abstract/Utils/WorldUtlis.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>

auto combatLog = spdlog::stdout_color_mt("combat");

void CombatSystem::update()
{
	auto view = manager.view<BattleManagerComponent>();
	if (view.archetypes.size() == 0) {
		return;
	} else {
		view.each([&](EntityID battleId, BattleManagerComponent &bmc) {
			auto player = WorldUtils::getPlayer(manager);
			EntityID playerId;
			if (player.has_value()) {
				playerId = player.value();
			}
			EntityID currentAttacker = this->getAttacker(bmc);
			BattleComponent &battle = manager.getComponent<BattleComponent>(currentAttacker);
			if (bmc.isBattleOver) {
				cleanUpBattle(battleId, battle.faction, battle.battleState);
				spdlog::get("combat")->info("Cleanup is done");
				auto playerOpt = WorldUtils::getPlayer(manager);
				if (playerOpt.has_value()) {
					spdlog::get("combat")->info("Player is here!");
				} else {
					spdlog::error("Player was accidentally DESTROYED!");
				}
				return;
			}
			battle.isActiveTurn = true;
			switch (battle.battleState) {
			case BattleState::TURN_START: {

				battle.battleState = BattleState::WAITING_FOR_INPUT;
				auto &attackerBattleComp = manager.getComponent<BattleComponent>(currentAttacker);
				if (attackerBattleComp.controller == BATTLE_CONTROLLER::AI) {
					aiSystem.executeAILogic(currentAttacker, bmc.participants);
				}

				break;
			}
			case BattleState::SELECTED_ACTION:
				this->executeBattleAction(currentAttacker, battle.target, battle.selectedAction);
				battle.battleState = BattleState::EXECUTING_ACTION;
				break;
			case BattleState::EXECUTING_ACTION:
				if (this->handleActionDelay(battle)) {
					battle.battleState = BattleState::CHECK_DEATH;
				}
				break;
			case BattleState::CHECK_DEATH: {

				auto result = this->checkDeathCondition(battle.target, currentAttacker);
				if (result == BattleState::VICTORY) {
					audioSystem.enqueueSound("victory_sound");
					bmc.currentTurnIndex = 0;
					manager.getComponent<BattleComponent>(playerId).battleState = BattleState::STATS_DISTRIBUTION;
				} else {
					battle.battleState = result;
				}
				break;
			}
			case BattleState::NEXT_ROUND:
				this->passTurn(currentAttacker, bmc);
				break;
			case BattleState::VICTORY:
				spdlog::get("combat")->info("Victory for player");
				bmc.isBattleOver = true;
				break;
			case BattleState::DEFEAT:
				spdlog::get("combat")->info("Defeat! for player");
				bmc.isBattleOver = true;
				break;
			case BattleState::STATS_DISTRIBUTION:
				break;
			}
		});
	}
}

void CombatSystem::executeBattleAction(EntityID attacker, EntityID defender, BattleAction typeOfAction)
{
	auto &attackerBattle = manager.getComponent<BattleComponent>(attacker);

	int cost = getActionCost(typeOfAction);
	if (attackerBattle.AP < cost) {
		spdlog::get("combat")->warn("Not enough AP to do this action");
		attackerBattle.battleState = BattleState::WAITING_FOR_INPUT;
		return;
	}
	attackerBattle.AP -= cost;

	if (typeOfAction == BattleAction::ULTIMATE_ATTACK and attackerBattle.numberOfUltimateAttacksUsed >= 1) {
		spdlog::get("combat")->warn("No ultimate attacks left!");
		attackerBattle.battleState = BattleState::WAITING_FOR_INPUT;
		return;
	}

	if (typeOfAction == BattleAction::HEAL and attackerBattle.numberOfHealsUsed >= 2) {
		spdlog::get("combat")->warn("No heals left!");
		attackerBattle.battleState = BattleState::WAITING_FOR_INPUT;
		return;
	}

	switch (typeOfAction) {

	case BattleAction::LIGHT_ATTACK: {
		auto &attackerStats = manager.getComponent<StatsComponent>(attacker);
		auto &inventory = this->manager.getComponent<InventoryComponent>(attacker);
		auto weaponId = inventory.getEquippedItem(ITEM_TYPE::WEAPON);
		auto attackerWeapon = manager.getComponent<WeaponComponent>(weaponId);
		float damage = getDamageWithScaling(attackerStats, attackerWeapon, typeOfAction);
		audioSystem.enqueueSound(attackerWeapon.hitSoundLight);
		spdlog::get("combat")->info("Light Damage: {}", damage);
		auto health = manager.getComponent<StatsComponent>(defender).health;
		manager.getComponent<StatsComponent>(defender).health = std::max(0.0f, health - damage);
		manager.getComponent<StateComponent>(defender).setState(LIGHT_HIT);
		break;
	}
	case BattleAction::HEAVY_ATTACK: {

		auto &attackerStats = manager.getComponent<StatsComponent>(attacker);
		auto &inventory = this->manager.getComponent<InventoryComponent>(attacker);
		auto weaponId = inventory.getEquippedItem(ITEM_TYPE::WEAPON);
		auto attackerWeapon = manager.getComponent<WeaponComponent>(weaponId);
		float damage = getDamageWithScaling(attackerStats, attackerWeapon, typeOfAction);
		audioSystem.enqueueSound(attackerWeapon.hitSoundHeavy);
		spdlog::get("combat")->info("Heavy Damage: {}", damage);
		auto health = manager.getComponent<StatsComponent>(defender).health;
		manager.getComponent<StatsComponent>(defender).health = std::max(0.0f, health - damage);
		manager.getComponent<StateComponent>(defender).setState(HEAVY_HIT);
		break;
	}

	case BattleAction::ULTIMATE_ATTACK: {

		auto &attackerStats = manager.getComponent<StatsComponent>(attacker);
		auto &inventory = this->manager.getComponent<InventoryComponent>(attacker);
		auto weaponId = inventory.getEquippedItem(ITEM_TYPE::WEAPON);
		auto attackerWeapon = manager.getComponent<WeaponComponent>(weaponId);
		float damage = getDamageWithScaling(attackerStats, attackerWeapon, typeOfAction);
		audioSystem.enqueueSound(attackerWeapon.hitSoundUltimate);
		spdlog::get("combat")->info("Ultimate Damage: {}", damage);
		auto health = manager.getComponent<StatsComponent>(defender).health;
		manager.getComponent<StatsComponent>(defender).health = std::max(0.0f, health - damage);
		manager.getComponent<BattleComponent>(attacker).numberOfUltimateAttacksUsed += 1;
		manager.getComponent<StateComponent>(defender).setState(ULTIMATE_HIT);
		break;
	}
	case BattleAction::HEAL: {

		auto &attackerStats = manager.getComponent<StatsComponent>(attacker);
		audioSystem.enqueueSound("heal_sound");
		this->takeHealAction(attacker, attackerStats.getStat(STATS::FAITH), attackerStats.getStat(STATS::MAX_HEALTH));
		manager.getComponent<BattleComponent>(attacker).numberOfHealsUsed += 1;
		break;
	}
	case BattleAction::REST: {
		audioSystem.enqueueSound("rest_sound");
		this->restoreAP(attacker);
		break;
	}
	}
	if (defender == -1) {
		throw std::runtime_error("Defender is not set for action execution");
	}
	bool isOffensive = (typeOfAction == BattleAction::LIGHT_ATTACK || typeOfAction == BattleAction::HEAVY_ATTACK
	                    || typeOfAction == BattleAction::ULTIMATE_ATTACK);

	if (isOffensive) {
		spdlog::get("combat")->info("Entity {} attacks Entity {} with action {}!", attacker.getId(), defender.getId(),
		                            static_cast<int>(typeOfAction));

		spdlog::get("combat")->info("Defender Entity {} has {} HP and {} AP left!", defender.getId(),
		                            manager.getComponent<StatsComponent>(defender).health,
		                            manager.getComponent<BattleComponent>(defender).AP);
	} else {
		std::string actionName = (typeOfAction == BattleAction::HEAL) ? "heals" : "rests";
		spdlog::get("combat")->info("Entity {} {}!", attacker.getId(), actionName);
	}

	spdlog::get("combat")->info("Attacker Entity {} has {} HP and {} AP left!", attacker.getId(),
	                            manager.getComponent<StatsComponent>(attacker).health,
	                            manager.getComponent<BattleComponent>(attacker).AP);
}

void CombatSystem::takeHealAction(EntityID healer, int faith, int maxHealth)
{
	StatsComponent &statsComponent = manager.getComponent<StatsComponent>(healer);

	float healAmount = maxHealth * faith * 10.0 / 100.0f;
	statsComponent.health = std::min((float)maxHealth, statsComponent.health + healAmount);
}

void CombatSystem::restoreAP(EntityID restorator)
{
	BattleComponent &battleComponent = manager.getComponent<BattleComponent>(restorator);
	battleComponent.AP += 2;
}

bool CombatSystem::handleActionDelay(BattleComponent &battle)
{
	float dt = clock.restart().asSeconds();

	battle.actionTimer += dt;

	if (battle.actionTimer >= battle.actionDelay) {
		battle.actionTimer = 0.0f;
		return true;
	}

	return false;
}
BattleState CombatSystem::checkDeathCondition(EntityID defender, EntityID attacker)
{
	float healthDefender = manager.getComponent<StatsComponent>(defender).health;
	if (healthDefender > 0.0) {
		return BattleState::NEXT_ROUND;
	}
	auto player = WorldUtils::getPlayer(manager);
	EntityID playerId;
	if (player.has_value()) {
		playerId = player.value();
	} else {
		throw std::runtime_error("No player found in checkDeathCondition");
	}
	auto &defenderBattleComp = manager.getComponent<BattleComponent>(defender);

	if (defenderBattleComp.faction == BATTLE_FACTION::PLAYER_PARTY) {
		if (defender == playerId && manager.hasComponent<CombatGodMode>(playerId)) {
			manager.getComponent<StatsComponent>(defender).health = 100;
			spdlog::get("combat")->info("Healing player back to 100, because of god mode");
			return BattleState::NEXT_ROUND;
		}
		manager.addComponentToEntity<DeathComponent>(defender);
		auto &dc = manager.getComponent<DeathComponent>(defender);
		dc.graveTile = GraveConfig::defaultTile;
		dc.graveTilesetPath = GraveConfig::tilesetPath;

		if (defender == playerId) {
			return BattleState::DEFEAT;
		}
	} else {
		manager.addComponentToEntity<DeathComponent>(defender);
		auto &dc = manager.getComponent<DeathComponent>(defender);
		dc.graveTile = GraveConfig::defaultTile;
		dc.graveTilesetPath = GraveConfig::tilesetPath;
		audioSystem.enqueueSound("enemy_death_sound");
	}
	auto &attackerBattleComp = manager.getComponent<BattleComponent>(attacker);

	std::vector<EntityID> aliveEnemies =
	    BattleInputSystem::getTargetsInBattle(playerId, attackerBattleComp.battleManagerId, this->manager);

	if (aliveEnemies.empty()) {
		return BattleState::VICTORY;
	}
	return BattleState::NEXT_ROUND;
}
void CombatSystem::passTurn(EntityID &currentEntity, BattleManagerComponent &bmc)
{

	manager.getComponent<BattleComponent>(currentEntity).isActiveTurn = false;
	bmc.currentTurnIndex++;
	EntityID nextId = this->getAttacker(bmc);
	auto &nextBattleComponent = manager.getComponent<BattleComponent>(nextId);
	nextBattleComponent.battleState = BattleState::TURN_START;
	nextBattleComponent.isActiveTurn = true;
}
EntityID CombatSystem::getAttacker(BattleManagerComponent &bmc)
{
	if (bmc.participants.size() == 0) {
		throw std::runtime_error("No participants in battle");
	}
	int numberOfParticipants = bmc.participants.size();
	EntityID attacker = bmc.participants[bmc.currentTurnIndex % numberOfParticipants];
	if (!manager.hasComponent<DeathComponent>(attacker)) {
		return attacker;
	}
	int loopSafeguard = 0;
	while (manager.hasComponent<DeathComponent>(attacker)) {
		bmc.currentTurnIndex++;
		loopSafeguard++;
		if (loopSafeguard > numberOfParticipants) {
			throw std::runtime_error("All participants are dead, but battle is not over");
		}
		attacker = bmc.participants[bmc.currentTurnIndex % numberOfParticipants];
	}
	return attacker;
}
void CombatSystem::cleanUpBattle(EntityID battleManagerId, BATTLE_FACTION winningBattleFaction, BattleState battleState)
{
	auto &bmc = manager.getComponent<BattleManagerComponent>(battleManagerId);
	auto playerIdOpt = WorldUtils::getPlayer(manager);
	if (!playerIdOpt.has_value())
		return;
	std::vector<EntityID> defeatedEnemies;
	auto participantsCopy = bmc.participants;

	for (EntityID entity : participantsCopy) {
		BATTLE_FACTION battleFaction = manager.getComponent<BattleComponent>(entity).faction;
		manager.removeComponentFromEntity<BattleComponent>(entity);
		if (battleFaction == winningBattleFaction) {
			auto &stats = manager.getComponent<StatsComponent>(entity);
			if (stats.health > stats.getStat(STATS::MAX_HEALTH)) {
				spdlog::get("combat")->warn(
				    "HP of Entity {} have been set to max_health {}, which is lower than current health {}",
				    entity.getId(), stats.getStat(STATS::MAX_HEALTH), stats.health);
			}
			stats.health = stats.getStat(STATS::MAX_HEALTH);
			stats.experienceLevel += 1;
			stats.numberOfFightsWon += 1;

			// TODO: Do we want companions to be permantly death?
			if (manager.hasComponent<DeathComponent>(entity)) {
				manager.removeComponentFromEntity<DeathComponent>(entity);
			}
			// Hide companions again
			if (entity != playerIdOpt.value()) {
				if (manager.hasComponent<PartOfLayerComponent>(entity)) {
					manager.removeComponentFromEntity<PartOfLayerComponent>(entity);
				}
			}
		} else {
			defeatedEnemies.push_back(entity);
		}
	}
	manager.destroyEntity(battleManagerId);

	if (battleState == BattleState::VICTORY) {
		manager.addComponentToEntity<InputComponent>(playerIdOpt.value());
		for (EntityID defeatedEnemy : defeatedEnemies) {
			manager.destroyEntity(defeatedEnemy);
		}
		spdlog::get("combat")->info("You won the battle!");

	} else if (battleState == BattleState::DEFEAT) {
		// port away from enemy Fix for now
		audioSystem.enqueueSound("defeat_sound");

		auto &trans = manager.getComponent<TransformComponent>(playerIdOpt.value());
		trans.position = {0, 1};
		manager.getComponent<StateComponent>(playerIdOpt.value()).setState(DIE, true);
		spdlog::get("combat")->info("You lost the battle! Game over");
	}
	audioSystem.switchMusic("overworld", true);
}

float CombatSystem::getDamageWithScaling(const StatsComponent &statsComponent, const WeaponComponent &weaponComponent,
                                         BattleAction action)
{
	if (action == BattleAction::LIGHT_ATTACK) {
		return weaponComponent.lightAttackBaseDmg
		       + getMultiplicatorFromScalingFactor(statsComponent, weaponComponent) * 1.0f;
	} else if (action == BattleAction::HEAVY_ATTACK) {
		return weaponComponent.heavyAttackBaseDmg
		       + getMultiplicatorFromScalingFactor(statsComponent, weaponComponent) * 2.0f;
	} else if (action == BattleAction::ULTIMATE_ATTACK) {
		return weaponComponent.ultimateAttackBaseDmg
		       + getMultiplicatorFromScalingFactor(statsComponent, weaponComponent) * 3.0f;
	}
	throw std::runtime_error("Invalid weapon type");
}

float CombatSystem::getMultiplicatorFromScalingFactor(StatsComponent stats, const WeaponComponent &weaponComponent)
{
	return stats.getStat(weaponComponent.scalingStat) * weaponComponent.getScalingFactor();
}

int CombatSystem::getActionCost(BattleAction action)
{
	switch (action) {
	case BattleAction::LIGHT_ATTACK:
		return 1;
	case BattleAction::HEAVY_ATTACK:
		return 2;
	case BattleAction::HEAL:
		return 2;
	default:
		return 0;
	}
}

bool CombatSystem::validateAction(BattleAction action, const BattleComponent &battle)

{
	int cost = getActionCost(action);
	if (battle.AP < cost) {
		return false;
	}
	if (action == BattleAction::ULTIMATE_ATTACK && battle.numberOfUltimateAttacksUsed >= battle.maxUltimateAttacks) {
		return false;
	}

	if (action == BattleAction::HEAL && battle.numberOfHealsUsed >= battle.maxHeals) {
		return false;
	}
	return true;
}
