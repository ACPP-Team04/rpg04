#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Combat/Components/CombatGodMode.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/StateComponent.hpp"
#include "Abstract/PersistenceManager/PersistenceManager.hpp"
#include "Abstract/Utils/GraveConfig.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "Implementation/Components/BattleComponent.hpp"

#include <Abstract/Combat/Components/DeathComponent.hpp>
#include <Abstract/Combat/Components/HitFeedbackComponent.hpp>
#include <Abstract/Combat/Components/LungeComponent.hpp>
#include <Abstract/Combat/Systems/BattleInputSystem.hpp>
#include <Abstract/Exception/AllParticipantsDeadException.hpp>
#include <Abstract/Exception/InvalidAttackActionSelectedException.hpp>
#include <Abstract/Exception/InvalidCombatTargetException.hpp>
#include <Abstract/Exception/NoParticipantsException.hpp>
#include <Abstract/Exception/PlayerNotFoundException.hpp>
#include <Abstract/Overwordl/Components/InputComponent.hpp>
#include <Abstract/Overwordl/Components/ItemComponent.hpp>
#include <Abstract/Overwordl/Components/TransformComponent.hpp>
#include <Abstract/TILE_ENUMS.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

const auto combatLog = spdlog::stdout_color_mt("combat");

void CombatSystem::update()
{
	auto view = manager.view<BattleManagerComponent>();
	if (view.archetypes.empty()) {
		return;
	} else {
		view.each([this](const EntityID &battleId, BattleManagerComponent &bmc) { processBattleTick(battleId, bmc); });
	}
}

void CombatSystem::processBattleTick(const EntityID &battleId, BattleManagerComponent &bmc)
{
	auto player = WorldUtils::getPlayer(manager);
	EntityID playerId;
	if (player.has_value()) {
		playerId = player.value();
	}
	EntityID currentAttacker = this->getAttacker(bmc);
	BattleComponent &battle = manager.getComponent<BattleComponent>(currentAttacker);
	if (bmc.isBattleOver) {
		cleanUpBattle(battleId, battle.faction, battle.battleState);
		return;
	}
	battle.isActiveTurn = true;
	switch (battle.battleState) {
		using enum BattleState;
	case TURN_START:
		battle.battleState = WAITING_FOR_INPUT;
		if (battle.controller == BATTLE_CONTROLLER::AI) {
			aiSystem.executeAILogic(currentAttacker, bmc.participants);
		}
		break;
	case SELECTED_ACTION: {
		if (battle.selectedAction != BattleAction::REST && battle.selectedAction != BattleAction::HEAL) {
			this->setupKineticLunge(currentAttacker, battle.target);
		}
		BattleComponent &updatedBattle = manager.getComponent<BattleComponent>(currentAttacker);
		this->executeBattleAction(currentAttacker, updatedBattle.target, updatedBattle.selectedAction);
		updatedBattle.battleState = EXECUTING_ACTION;
		break;
	}
	case EXECUTING_ACTION: {

		if (manager.hasComponent<LungeComponent>(currentAttacker)
		    || manager.hasComponent<HitFeedbackComponent>(battle.target)) {
			break;
		}
		if (this->handleActionDelay(battle)) {
			battle.battleState = CHECK_DEATH;
		}
		break;
	}
	case CHECK_DEATH: {
		if (auto result = this->checkDeathCondition(battle.target, currentAttacker); result == VICTORY) {
			audioSystem.enqueueSound("victory_sound");
			bmc.currentTurnIndex = 0;
			manager.getComponent<BattleComponent>(playerId).battleState = STATS_DISTRIBUTION;
		} else {
			battle.battleState = result;
		}
		break;
	}
	case NEXT_ROUND:
		this->passTurn(currentAttacker, bmc);
		break;
	case VICTORY:
		bmc.isBattleOver = true;
		break;
	case DEFEAT:
		bmc.isBattleOver = true;
		break;
	case STATS_DISTRIBUTION:
		break;
	case WAITING_FOR_INPUT:
	case SELECTING_TARGET:
		// Intentionally ignored by CombatSystem
		//  WAITING_FOR_INPUT and SELECTING_TARGET are managed by the BattleInputSystem.
		break;
	}
}

int CombatSystem::getActionCost(BattleAction action)
{
	switch (action) {
		using enum BattleAction;
	case LIGHT_ATTACK:
		return 1;
	case HEAVY_ATTACK:
		return 2;
	case HEAL:
		return 2;
	default:
		return 0;
	}
}

bool CombatSystem::validateAction(BattleAction action, const BattleComponent &battle)
{
	if (battle.AP < static_cast<float>(getActionCost(action))) {
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

	auto cost = static_cast<float>(getActionCost(typeOfAction));
	if (attackerBattle.AP < cost) {
		spdlog::get("combat")->warn("Not enough AP to do this action");
		attackerBattle.battleState = BattleState::WAITING_FOR_INPUT;
		return;
	}

	if (typeOfAction == BattleAction::ULTIMATE_ATTACK && attackerBattle.numberOfUltimateAttacksUsed >= 1) {
		spdlog::get("combat")->warn("No ultimate attacks left!");
		attackerBattle.battleState = BattleState::WAITING_FOR_INPUT;
		return;
	}

	if (typeOfAction == BattleAction::HEAL && attackerBattle.numberOfHealsUsed >= 2) {
		spdlog::get("combat")->warn("No heals left!");
		attackerBattle.battleState = BattleState::WAITING_FOR_INPUT;
		return;
	}

	bool isOffensive = (typeOfAction == BattleAction::LIGHT_ATTACK || typeOfAction == BattleAction::HEAVY_ATTACK
	                    || typeOfAction == BattleAction::ULTIMATE_ATTACK);

	if (defender.getId() == -1) {
		throw InvalidCombatTargetException("Defender is not set for action execution");
	}
	const bool oneShotEnemyInGodMode =
	    isOffensive && manager.hasComponent<CombatGodMode>(attacker)
	    && manager.getComponent<BattleComponent>(defender).faction == BATTLE_FACTION::ENEMY;
	attackerBattle.AP -= cost;

	switch (typeOfAction) {
	case BattleAction::LIGHT_ATTACK: {
		const CharacterComponent &attackerCharacter = manager.getComponent<CharacterComponent>(attacker);
		CharacterComponent &defenderCharacter = manager.getComponent<CharacterComponent>(defender);
		auto weaponId = attackerCharacter.equipedWeapon;
		const auto &attackerWeapon = manager.getComponent<ItemComponent>(weaponId).weaponStats;
		auto damage = static_cast<int>(getDamageWithScaling(attackerCharacter.stats, attackerWeapon, typeOfAction));
		if (oneShotEnemyInGodMode)
			damage = defenderCharacter.stats.health;
		audioSystem.enqueueSound(attackerWeapon.hitSoundLight);
		spdlog::get("combat")->info("Light Damage: {}", damage);
		defenderCharacter.stats.health = std::max(0, defenderCharacter.stats.health - damage);
		manager.getComponent<StateComponent>(defender).setState(LIGHT_HIT);
		manager.addComponentToEntity<HitFeedbackComponent>(defender);
		break;
	}
	case BattleAction::HEAVY_ATTACK: {
		const CharacterComponent &attackerCharacter = manager.getComponent<CharacterComponent>(attacker);
		CharacterComponent &defenderCharacter = manager.getComponent<CharacterComponent>(defender);
		auto weaponId = attackerCharacter.equipedWeapon;
		const auto &attackerWeapon = manager.getComponent<ItemComponent>(weaponId).weaponStats;
		auto damage = static_cast<int>(getDamageWithScaling(attackerCharacter.stats, attackerWeapon, typeOfAction));
		if (oneShotEnemyInGodMode)
			damage = defenderCharacter.stats.health;
		audioSystem.enqueueSound(attackerWeapon.hitSoundHeavy);
		spdlog::get("combat")->info("Heavy Damage: {}", damage);
		defenderCharacter.stats.health = std::max(0, defenderCharacter.stats.health - damage);
		manager.getComponent<StateComponent>(defender).setState(HEAVY_HIT);
		manager.addComponentToEntity<HitFeedbackComponent>(defender);
		break;
	}
	case BattleAction::ULTIMATE_ATTACK: {
		const CharacterComponent &attackerCharacter = manager.getComponent<CharacterComponent>(attacker);
		CharacterComponent &defenderCharacter = manager.getComponent<CharacterComponent>(defender);
		auto weaponId = attackerCharacter.equipedWeapon;
		const auto &attackerWeapon = manager.getComponent<ItemComponent>(weaponId).weaponStats;
		auto damage = static_cast<int>(getDamageWithScaling(attackerCharacter.stats, attackerWeapon, typeOfAction));
		if (oneShotEnemyInGodMode)
			damage = defenderCharacter.stats.health;
		audioSystem.enqueueSound(attackerWeapon.hitSoundUltimate);
		spdlog::get("combat")->info("Ultimate Damage: {}", damage);
		defenderCharacter.stats.health = std::max(0, defenderCharacter.stats.health - damage);
		manager.getComponent<BattleComponent>(attacker).numberOfUltimateAttacksUsed += 1;
		manager.getComponent<StateComponent>(defender).setState(ULTIMATE_HIT);
		manager.addComponentToEntity<HitFeedbackComponent>(defender);
		break;
	}
	case BattleAction::HEAL: {
		const CharacterComponent &attackerCharacter = manager.getComponent<CharacterComponent>(attacker);
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

	if (isOffensive) {
		CharacterComponent &defenderCharacter = manager.getComponent<CharacterComponent>(defender);
		CharacterComponent &attackerCharacter = manager.getComponent<CharacterComponent>(attacker);
		spdlog::get("combat")->info("Entity {} attacks Entity {} with action {}!", attacker.getId(), defender.getId(),
		                            static_cast<int>(typeOfAction));
		spdlog::get("combat")->info("Defender Entity {} has {} HP and {} AP left!", defender.getId(),
		                            defenderCharacter.stats.health, manager.getComponent<BattleComponent>(defender).AP);
		spdlog::get("combat")->info("Attacker Entity {} has {} HP and {} AP left!", attacker.getId(),
		                            attackerCharacter.stats.health, manager.getComponent<BattleComponent>(attacker).AP);
	} else {
		std::string actionName = (typeOfAction == BattleAction::HEAL) ? "heals" : "rests";
		spdlog::get("combat")->info("Entity {} {}!", attacker.getId(), actionName);
		CharacterComponent &attackerCharacter = manager.getComponent<CharacterComponent>(attacker);
		spdlog::get("combat")->info("Attacker Entity {} has {} HP and {} AP left!", attacker.getId(),
		                            attackerCharacter.stats.health, manager.getComponent<BattleComponent>(attacker).AP);
	}
}

void CombatSystem::takeHealAction(EntityID healer, int faith, int maxHealth)
{
	CharacterComponent &character = manager.getComponent<CharacterComponent>(healer);
	float healAmount = static_cast<float>(maxHealth) * static_cast<float>(faith) * 10.0f / 100.0f;
	character.stats.health = std::min(maxHealth, character.stats.health + static_cast<int>(healAmount));
}

BattleState CombatSystem::checkDeathCondition(EntityID defender, EntityID attacker)
{
	if (static_cast<float>(manager.getComponent<CharacterComponent>(defender).stats.health) > 0.0f) {
		return BattleState::NEXT_ROUND;
	}
	auto player = WorldUtils::getPlayer(manager);
	EntityID playerId;
	if (player.has_value()) {
		playerId = player.value();
	} else {
		throw PlayerNotFoundException("No player found in checkDeathCondition");
	}

	if (manager.getComponent<BattleComponent>(defender).faction == BATTLE_FACTION::PLAYER_PARTY) {
		if (defender == playerId && manager.hasComponent<CombatGodMode>(playerId)) {
			manager.getComponent<CharacterComponent>(defender).stats.health = 100;
			spdlog::get("combat")->info("Healing player back to 100, because of god mode");
			return BattleState::NEXT_ROUND;
		}
		manager.addComponentToEntity<DeathComponent>(defender);
		auto &dc = manager.getComponent<DeathComponent>(defender);
		dc.graveTile = GraveConfig::defaultTile;
		dc.graveTilesetPath = GraveConfig::tilesetPath;
		manager.getComponent<StateComponent>(defender).setState(DIE);
		if (defender == playerId) {
			return BattleState::DEFEAT;
		}
	} else {
		manager.addComponentToEntity<DeathComponent>(defender);
		auto &dc = manager.getComponent<DeathComponent>(defender);
		dc.graveTile = GraveConfig::defaultTile;
		dc.graveTilesetPath = GraveConfig::tilesetPath;
		manager.getComponent<StateComponent>(defender).setState(DIE);
		audioSystem.enqueueSound("enemy_death_sound");
	}
	const auto &attackerBattleComp = manager.getComponent<BattleComponent>(attacker);

	if (BattleInputSystem::getTargetsInBattle(playerId, attackerBattleComp.battleManagerId, this->manager).empty()) {
		return BattleState::VICTORY;
	}
	return BattleState::NEXT_ROUND;
}

void CombatSystem::cleanUpBattle(EntityID battleManagerId, BATTLE_FACTION winningBattleFaction, BattleState battleState)
{
	const auto &bmc = manager.getComponent<BattleManagerComponent>(battleManagerId);
	auto playerIdOpt = WorldUtils::getPlayer(manager);
	if (!playerIdOpt.has_value())
		return;
	std::vector<EntityID> defeatedEnemies;
	const auto &participantsCopy = bmc.participants;

	for (EntityID entity : participantsCopy) {
		BATTLE_FACTION battleFaction = manager.getComponent<BattleComponent>(entity).faction;
		manager.removeComponentFromEntity<BattleComponent>(entity);
		if (battleFaction == winningBattleFaction) {
			handleEntityOfWinningFaction(entity, playerIdOpt.value());
		} else {
			defeatedEnemies.push_back(entity);
		}
	}
	manager.destroyEntity(battleManagerId);

	if (battleState == BattleState::VICTORY) {

		for (EntityID defeatedEnemy : defeatedEnemies) {
			manager.destroyEntity(defeatedEnemy);
		}
		manager.addComponentToEntity<InputComponent>(playerIdOpt.value());
		spdlog::get("combat")->info("You won the battle!");

	} else if (battleState == BattleState::DEFEAT) {
		audioSystem.enqueueSound("defeat_sound");

		manager.getComponent<StateComponent>(playerIdOpt.value()).setState(DIE, true);
		PersistenceManager::getInstance().requestGameOver = true;
		spdlog::get("combat")->info("You lost the battle! Game over");
	}
	WorldUtils::playMusicForCurrentGroup(manager);
}

void CombatSystem::handleEntityOfWinningFaction(const EntityID &entity, const EntityID &playerId)
{
	auto &stats = manager.getComponent<CharacterComponent>(entity).stats;
	if (stats.health > stats.getStat(STATS::MAX_HEALTH)) {
		spdlog::get("combat")->warn(
		    "HP of Entity {} have been set to max_health {}, which is lower than current health {}", entity.getId(),
		    stats.getStat(STATS::MAX_HEALTH), stats.health);
	}
	stats.health = stats.getStat(STATS::MAX_HEALTH);
	stats.experienceLevel += 1;
	stats.numberOfFightsWon += 1;

	// No perma-death for companions
	if (manager.hasComponent<DeathComponent>(entity)) {
		manager.removeComponentFromEntity<DeathComponent>(entity);
		manager.getComponent<StateComponent>(entity).setState(ENTITY_ANIMATIONS_STATE::IDLE);
	}
	const auto &playerChar = manager.getComponent<CharacterComponent>(playerId);
	// Hide companions again
	if (entity.getId() == playerChar.equipedCompanion) {
		moveCompanionToInventory(entity, playerChar.inventory.inventoryWorldId);
	}
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

void CombatSystem::passTurn(const EntityID &currentEntity, BattleManagerComponent &bmc)
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
	if (bmc.participants.empty()) {
		throw NoParticipantsException("No participants in battle");
	}
	auto numberOfParticipants = bmc.participants.size();
	auto &attacker = bmc.participants[bmc.currentTurnIndex % numberOfParticipants];
	if (!manager.hasComponent<DeathComponent>(attacker)) {
		return attacker;
	}
	int loopSafeguard = 0;
	while (manager.hasComponent<DeathComponent>(attacker)) {
		bmc.currentTurnIndex++;
		loopSafeguard++;
		if (loopSafeguard > numberOfParticipants) {
			throw AllParticipantsDeadException("All participants are dead, but battle is not over");
		}
		attacker = bmc.participants[bmc.currentTurnIndex % numberOfParticipants];
	}
	return attacker;
}

float CombatSystem::getDamageWithScaling(const StatsComponent &statsComponent, const WeaponComponent &weaponComponent,
                                         BattleAction action) const
{
	using enum BattleAction;
	if (action == LIGHT_ATTACK) {
		return static_cast<float>(weaponComponent.lightAttackBaseDmg)
		       + getMultiplicatorFromScalingFactor(statsComponent, weaponComponent) * 1.0f;
	} else if (action == HEAVY_ATTACK) {
		return static_cast<float>(weaponComponent.heavyAttackBaseDmg)
		       + getMultiplicatorFromScalingFactor(statsComponent, weaponComponent) * 2.0f;
	} else if (action == ULTIMATE_ATTACK) {
		return static_cast<float>(weaponComponent.ultimateAttackBaseDmg)
		       + getMultiplicatorFromScalingFactor(statsComponent, weaponComponent) * 3.0f;
	}
	throw InvalidAttackActionSelectedException("Invalid attack action selected");
}

float CombatSystem::getMultiplicatorFromScalingFactor(const StatsComponent &stats,
                                                      const WeaponComponent &weaponComponent) const
{
	return static_cast<float>(stats.getStat(weaponComponent.scalingStat)) * weaponComponent.getScalingFactor();
}

void CombatSystem::setupKineticLunge(const EntityID &attacker, const EntityID &defender)
{
	const auto &attackerTransform = manager.getComponent<TransformComponent>(attacker);
	const auto &defenderTransform = manager.getComponent<TransformComponent>(defender);

	sf::Vector2f aPos = attackerTransform.position;
	sf::Vector2f dPos = defenderTransform.position;

	sf::Vector2f difference = dPos - aPos;
	float distance = std::sqrt((difference.x * difference.x) + (difference.y * difference.y));

	sf::Vector2f direction(0.0f, 0.0f);
	if (distance > 0.0f) {
		direction = sf::Vector2f(difference.x / distance, difference.y / distance);
	}

	auto &attackerState = manager.getComponent<StateComponent>(attacker);

	if (std::abs(difference.x) > std::abs(difference.y)) {
		attackerState.setState((difference.x > 0) ? WALK_RIGHT : WALK_LEFT);
	} else {
		attackerState.setState((difference.y > 0) ? WALK_DOWN : WALK_UP);
	}

	float stopDistance = 16.0f;
	manager.addComponentToEntity<LungeComponent>(attacker);
	LungeComponent &lunge = manager.getComponent<LungeComponent>(attacker);

	lunge.originalPosition = aPos;
	lunge.targetPosition = dPos - (direction * stopDistance);
	lunge.targetEntity = defender;
}

void CombatSystem::moveCompanionToInventory(const EntityID &entity, const int inventoryWorldId)
{
	if (manager.hasComponent<PartOfLayerComponent>(entity)) {
		auto &partOfLayer = manager.getComponent<PartOfLayerComponent>(entity);
		partOfLayer.groupId = inventoryWorldId;
	}
}
