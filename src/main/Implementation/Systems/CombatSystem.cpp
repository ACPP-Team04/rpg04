#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Combat/Components/CombatGodMode.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/ItemComponent.hpp"
#include "Abstract/Overwordl/Components/StateComponent.hpp"
#include "Abstract/Utils/GraveConfig.hpp"
#include "Implementation/Components/BattleComponent.hpp"

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
				cleanUpBattle(battleId, currentAttacker, battle.battleState);
				return;
			}
			battle.isActiveTurn = true;
			switch (battle.battleState) {
			case BattleState::TURN_START:
				battle.battleState = BattleState::WAITING_FOR_INPUT;
				if (currentAttacker != playerId) {
					aiSystem.executeAILogic(currentAttacker, bmc.participants);
				}
				break;
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
					battle.battleState = BattleState::STATS_DISTRIBUTION;
				} else {
					battle.battleState = result;
				}
				break;
			}
			case BattleState::NEXT_ROUND:
				this->passTurn(currentAttacker, bmc);
				break;
			case BattleState::VICTORY:
				bmc.isBattleOver = true;
				break;
			case BattleState::DEFEAT:
				bmc.isBattleOver = true;
				break;
			case BattleState::STATS_DISTRIBUTION:
				break;
			}
		});
	}
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
		CharacterComponent &attackerCharacter = manager.getComponent<CharacterComponent>(attacker);
		CharacterComponent &defenderCharacter = manager.getComponent<CharacterComponent>(defender);
		auto weaponId = attackerCharacter.equipedWeapon;
		auto attackerWeapon = manager.getComponent<ItemComponent>(weaponId).weaponStats;
		float damage = getDamageWithScaling(attackerCharacter.stats, attackerWeapon, typeOfAction);
		audioSystem.enqueueSound(attackerWeapon.hitSoundLight);
		spdlog::get("combat")->info("Light Damage: {}", damage);
		defenderCharacter.stats.health = std::max(0.0f, defenderCharacter.stats.health - damage);
		manager.getComponent<StateComponent>(defender).setState(LIGHT_HIT);
		break;
	}
	case BattleAction::HEAVY_ATTACK: {
		CharacterComponent &attackerCharacter = manager.getComponent<CharacterComponent>(attacker);
		CharacterComponent &defenderCharacter = manager.getComponent<CharacterComponent>(defender);
		auto weaponId = attackerCharacter.equipedWeapon;
		auto attackerWeapon = manager.getComponent<ItemComponent>(weaponId).weaponStats;
		float damage = getDamageWithScaling(attackerCharacter.stats, attackerWeapon, typeOfAction);
		audioSystem.enqueueSound(attackerWeapon.hitSoundHeavy);
		spdlog::get("combat")->info("Heavy Damage: {}", damage);
		defenderCharacter.stats.health = std::max(0.0f, defenderCharacter.stats.health - damage);
		manager.getComponent<StateComponent>(defender).setState(HEAVY_HIT);
		break;
	}
	case BattleAction::ULTIMATE_ATTACK: {
		CharacterComponent &attackerCharacter = manager.getComponent<CharacterComponent>(attacker);
		CharacterComponent &defenderCharacter = manager.getComponent<CharacterComponent>(defender);
		auto weaponId = attackerCharacter.equipedWeapon;
		auto attackerWeapon = manager.getComponent<ItemComponent>(weaponId).weaponStats;
		float damage = getDamageWithScaling(attackerCharacter.stats, attackerWeapon, typeOfAction);
		audioSystem.enqueueSound(attackerWeapon.hitSoundUltimate);
		spdlog::get("combat")->info("Ultimate Damage: {}", damage);
		defenderCharacter.stats.health = std::max(0.0f, defenderCharacter.stats.health - damage);
		manager.getComponent<BattleComponent>(attacker).numberOfUltimateAttacksUsed += 1;
		manager.getComponent<StateComponent>(defender).setState(ULTIMATE_HIT);
		break;
	}
	case BattleAction::HEAL: {
		CharacterComponent &attackerCharacter = manager.getComponent<CharacterComponent>(attacker);
		audioSystem.enqueueSound("heal_sound");
		this->takeHealAction(attacker, attackerCharacter.stats.getStat(STATS::FAITH),
		                     attackerCharacter.stats.getStat(STATS::MAX_HEALTH));
		manager.getComponent<BattleComponent>(attacker).numberOfHealsUsed += 1;
		break;
	}
	case BattleAction::REST: {
		audioSystem.enqueueSound("rest_sound");
		this->restoreAP(attacker);
		break;
	}
	}

	if (defender.getId() == -1) {
		throw std::runtime_error("Defender is not set for action execution");
	}

	bool isOffensive = (typeOfAction == BattleAction::LIGHT_ATTACK || typeOfAction == BattleAction::HEAVY_ATTACK
	                    || typeOfAction == BattleAction::ULTIMATE_ATTACK);

	if (isOffensive) {
		CharacterComponent &defenderCharacter = manager.getComponent<CharacterComponent>(defender);
		CharacterComponent &attackerCharacter = manager.getComponent<CharacterComponent>(attacker);
		spdlog::get("combat")->info("Entity {} attacks Entity {} with action {}!", attacker.getId(), defender.getId(),
		                            static_cast<int>(typeOfAction));
		spdlog::get("combat")->info("Defender Entity {} has {} HP and {} AP left!", defender.getId(),
		                            defenderCharacter.stats.health,
		                            manager.getComponent<BattleComponent>(defender).AP);
		spdlog::get("combat")->info("Attacker Entity {} has {} HP and {} AP left!", attacker.getId(),
		                            attackerCharacter.stats.health,
		                            manager.getComponent<BattleComponent>(attacker).AP);
	} else {
		std::string actionName = (typeOfAction == BattleAction::HEAL) ? "heals" : "rests";
		spdlog::get("combat")->info("Entity {} {}!", attacker.getId(), actionName);
		CharacterComponent &attackerCharacter = manager.getComponent<CharacterComponent>(attacker);
		spdlog::get("combat")->info("Attacker Entity {} has {} HP and {} AP left!", attacker.getId(),
		                            attackerCharacter.stats.health,
		                            manager.getComponent<BattleComponent>(attacker).AP);
	}
}

void CombatSystem::takeHealAction(EntityID healer, int faith, int maxHealth)
{
	CharacterComponent &character = manager.getComponent<CharacterComponent>(healer);
	float healAmount = maxHealth * faith * 10.0 / 100.0f;
	character.stats.health = std::min((float)maxHealth, character.stats.health + healAmount);
}

BattleState CombatSystem::checkDeathCondition(EntityID defender, EntityID attacker)
{
	float healthDefender = manager.getComponent<CharacterComponent>(defender).stats.health;
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
	bool playerIsAttacking = attacker == playerId;
	bool playerIsDefending = defender == playerId;

	if (healthDefender <= 0 && playerIsDefending) {
		if (manager.hasComponent<CombatGodMode>(playerId)) {
			manager.getComponent<CharacterComponent>(defender).stats.health = 100;
			spdlog::get("combat")->info("Healing player back to 100, because of god mode");
			return BattleState::NEXT_ROUND;
		}
		return BattleState::DEFEAT;
	} else if (healthDefender <= 0 && playerIsAttacking) {
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

void CombatSystem::cleanUpBattle(EntityID battleManagerId, EntityID winningEntity, BattleState battleState)
{
	auto &bmc = manager.getComponent<BattleManagerComponent>(battleManagerId);
	auto playerID = WorldUtils::getPlayer(manager);
	std::vector<EntityID> defeatedEnemies;

	manager.addComponentToEntity<InputComponent>(playerID.value());

	auto participantsCopy = bmc.participants;
	for (EntityID entity : participantsCopy) {
		manager.removeComponentFromEntity<BattleComponent>(entity);
		auto &character = manager.getComponent<CharacterComponent>(entity);
		auto &stats = character.stats;
		if (stats.health > stats.getStat(STATS::MAX_HEALTH)) {
			spdlog::get("combat")->warn(
			    "HP of Entity {} have been set to max_health {}, which is lower than current health {}",
			    entity.getId(), stats.getStat(STATS::MAX_HEALTH), stats.health);
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
		audioSystem.enqueueSound("defeat_sound");
		auto &trans = manager.getComponent<TransformComponent>(playerID.value());
		trans.position = {0, 1};
		spdlog::get("combat")->info("You lost the battle! Game over");
		manager.getComponent<StateComponent>(playerID.value()).setState(DIE, true);
	}
	audioSystem.switchMusic("overworld", true);
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
	auto attacker = bmc.participants[bmc.currentTurnIndex % numberOfParticipants];
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