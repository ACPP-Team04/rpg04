#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemHealstatsComponent.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/StatsComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
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
			EntityID currentAttacker = this->getAttacker(bmc.currentTurnIndex, bmc.participants);
			BattleComponent &battle = manager.getComponent<BattleComponent>(currentAttacker);
			if (bmc.isBattleOver) {
				cleanUpBattle(battleId, currentAttacker, battle.battleState);
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
				if (currentAttacker != playerId) {
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
					battle.battleState = BattleState::STATS_DISTRIBUTION;
				} else {
					battle.battleState = result;
				}
				break;
			}
			case BattleState::NEXT_ROUND:
				bmc.currentTurnIndex++;
				this->passTurn(currentAttacker, bmc.currentTurnIndex, bmc.participants);
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
	switch (typeOfAction) {

	case BattleAction::LIGHT_ATTACK: {
		auto &attackerStats = manager.getComponent<StatsComponent>(attacker);
		auto inventory = this->manager.getComponent<InventoryComponent>(attacker);
		auto weaponId = inventory.getEquippedItem(ITEM_TYPE::WEAPON);
		auto attackerWeapon = manager.getComponent<WeaponComponent>(weaponId);
		float damage = getDamageWithScaling(attackerStats, attackerWeapon, typeOfAction);
		spdlog::get("combat")->info("Damage: {}", damage);
		auto health = manager.getComponent<StatsComponent>(defender).health;
		manager.getComponent<StatsComponent>(defender).health = std::max(0.0f, health - damage);
		break;
	}
	case BattleAction::HEAVY_ATTACK: {

		auto &attackerStats = manager.getComponent<StatsComponent>(attacker);
		auto inventory = this->manager.getComponent<InventoryComponent>(attacker);
		auto weaponId = inventory.getEquippedItem(ITEM_TYPE::WEAPON);
		auto attackerWeapon = manager.getComponent<WeaponComponent>(weaponId);
		float damage = getDamageWithScaling(attackerStats, attackerWeapon, typeOfAction);
		auto health = manager.getComponent<StatsComponent>(defender).health;
		manager.getComponent<StatsComponent>(defender).health = std::max(0.0f, health - damage);
		break;
	}

	case BattleAction::ULTIMATE_ATTACK: {

		auto &attackerStats = manager.getComponent<StatsComponent>(attacker);
		auto inventory = this->manager.getComponent<InventoryComponent>(attacker);
		auto weaponId = inventory.getEquippedItem(ITEM_TYPE::WEAPON);
		auto attackerWeapon = manager.getComponent<WeaponComponent>(weaponId);
		float damage = getDamageWithScaling(attackerStats, attackerWeapon, typeOfAction);
		auto health = manager.getComponent<StatsComponent>(defender).health;
		manager.getComponent<StatsComponent>(defender).health = std::max(0.0f, health - damage);
		manager.getComponent<BattleComponent>(attacker).numberOfUltimateAttacksUsed += 1;
		break;
	}
	case BattleAction::HEAL: {

		auto &attackerStats = manager.getComponent<StatsComponent>(attacker);
		this->takeHealAction(attacker, attackerStats.getStat(STATS::FAITH), attackerStats.getStat(STATS::MAX_HEALTH));
		break;
	}
	case BattleAction::REST: {

		this->restoreAP(attacker);
		break;
	}
	}
	spdlog::get("combat")->info("Entity {} attacks Entity {} with {}!", attacker.getId(), defender.getId(),
	                            static_cast<int>(typeOfAction));
	spdlog::get("combat")->info("Entity {} has {} HP and {} AP left!", defender.getId(),
	                            manager.getComponent<StatsComponent>(defender).health,
	                            manager.getComponent<BattleComponent>(defender).AP);
	spdlog::get("combat")->info("Entity {} has {} HP and {} AP left!", attacker.getId(),
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
	auto player = WorldUtils::getPlayer(manager);
	EntityID playerId;
	if (player.has_value()) {
		playerId = player.value();
	}
	bool playerIsAttacking = attacker == playerId;

	if (healthDefender <= 0) {
		return playerIsAttacking ? BattleState::VICTORY : BattleState::DEFEAT;
	}

	return BattleState::NEXT_ROUND;
}
void CombatSystem::passTurn(EntityID &currentEntity, int currentTurnIndex, const std::vector<EntityID> participants)
{

	manager.getComponent<BattleComponent>(currentEntity).isActiveTurn = false;
	EntityID nextId = this->getAttacker(currentTurnIndex, participants);
	auto &nextBattleComponent = manager.getComponent<BattleComponent>(nextId);
	nextBattleComponent.battleState = BattleState::TURN_START;
	nextBattleComponent.isActiveTurn = true;
}
EntityID CombatSystem::getAttacker(int currentTurnIndex, const std::vector<EntityID> participants)
{
	if (participants.size() == 0) {
		throw std::runtime_error("No participants in battle");
	}
	return participants[currentTurnIndex % participants.size()];
}
void CombatSystem::cleanUpBattle(EntityID battleManagerId, EntityID winningEntity, BattleState battleState)
{
	auto &bmc = manager.getComponent<BattleManagerComponent>(battleManagerId);
	auto playerID = WorldUtils::getPlayer(manager);
	std::vector<EntityID> defeatedEnemies;

	manager.addComponentToEntity<InputComponent>(playerID.value());

	auto participantsCopy = bmc.participants;
	for (EntityID entity : participantsCopy) {
		manager.removeComponentFromEntity<BattleComponent>(entity);
		auto &stats = manager.getComponent<StatsComponent>(entity);
		if (stats.health > stats.getStat(STATS::MAX_HEALTH)) {
			spdlog::get("combat")->warn(
			    "HP of Entity {} have been set to max_health {}, which is lower than current health {}", entity.getId(),
			    stats.getStat(STATS::MAX_HEALTH), stats.health);
		}
		stats.health = stats.getStat(STATS::MAX_HEALTH);
		if (entity == winningEntity && battleState == BattleState::VICTORY) {
			stats.experienceLevel += 1;
			stats.numberOfFightsWon += 1;
		} else if (entity != playerID.value()) {
			defeatedEnemies.push_back(entity);
		}
	}
	manager.destroyEntity(battleManagerId);

	if (battleState == BattleState::VICTORY) {
		for (EntityID defeatedEnemy : defeatedEnemies) {
			manager.destroyEntity(defeatedEnemy);
		}
		spdlog::get("combat")->info("You won the battle!");

	} else if (battleState == BattleState::DEFEAT) {
		// port away from enemy
		auto &trans = manager.getComponent<TransformComponent>(playerID.value());
		trans.position = {0, 1};
		spdlog::get("combat")->info("You lost the battle! Game over");
	}
}

float CombatSystem::getDamageWithScaling(const StatsComponent &statsComponent, const WeaponComponent &weaponComponent,
                                         BattleAction action)
{
	if (action == BattleAction::LIGHT_ATTACK) {
		return weaponComponent.lightAttackBaseDmg + getMultiplicatorFromScalingFactor(statsComponent, weaponComponent);
	} else if (action == BattleAction::HEAVY_ATTACK) {
		return weaponComponent.heavyAttackBaseDmg + getMultiplicatorFromScalingFactor(statsComponent, weaponComponent);
	} else if (action == BattleAction::ULTIMATE_ATTACK) {
		return weaponComponent.ultimateAttackBaseDmg
		       + getMultiplicatorFromScalingFactor(statsComponent, weaponComponent);
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

bool CombatSystem::validateAction(BattleAction action, int AP, int numberOfUltimateAttacksUsed,
                                  int numberOfHealthPotions)
{
	int cost = getActionCost(action);
	if (AP < cost) {
		return false;
	}
	if (action == BattleAction::ULTIMATE_ATTACK && numberOfUltimateAttacksUsed >= 1) {
		return false;
	}
	if (action == BattleAction::HEAL && numberOfHealthPotions <= 0) {
		return false;
	}
	return true;
}
