#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/InventoryComponent.hpp"
#include "Implementation/Components/StatsComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

auto combatLog = spdlog::stdout_color_mt("combat");

// Check if there are entities in battleState if yes, deal with their plays
void CombatSystem::update()
{
	auto view = manager.view<BattleManagerComponent>();

	if (view.archetypes.size() == 0) {
		spdlog::get("combat")->warn("No BattleManagerComponent found");
		return;
	} else {
		view.each([&](EntityID battleId, BattleManagerComponent &bmc) {
			EntityID currentAttacker = this->getAttacker(bmc.currentTurnIndex, bmc.participants);
			if (bmc.isBattleOver) {
				cleanUpBattle(battleId, currentAttacker);
				return;
			}
			BattleComponent &battle = manager.getComponent<BattleComponent>(currentAttacker);
			battle.isActiveTurn = true;
			switch (battle.battleState) {
			case BattleState::TURN_START: {

				battle.battleState = BattleState::WAITING_FOR_INPUT;
				auto tag = manager.getEntityTag(currentAttacker);
				if (tag.has_value()) {
					if (tag.value() == EntityTag::ENEMY) {
						aiSystem.executeAILogic(currentAttacker, bmc.participants);
					}
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
			case BattleState::CHECK_DEATH:
				battle.battleState = this->checkDeathCondition(battle.target);
				break;
			case BattleState::NEXT_ROUND:
				bmc.currentTurnIndex++;
				this->passTurn(currentAttacker, bmc.currentTurnIndex, bmc.participants);
				break;
			case BattleState::VICTORY:
				// remove battle component from all entities in battle and set isActiveTurn to false
				spdlog::get("combat")->debug("Victory for player");
				bmc.isBattleOver = true;
				break;
			case BattleState::DEFEAT:
				// remove battle component from all entities in battle and set isActiveTurn to false
				spdlog::get("combat")->debug("Defeat! for player");
				bmc.isBattleOver = true;
				break;
			}
		});
	}
}

void CombatSystem::executeBattleAction(EntityID attacker, EntityID defender, BattleAction typeOfAction)
{
	auto &attackerBattle = manager.getComponent<BattleComponent>(attacker);
	auto &attackerInventory = manager.getComponent<InventoryComponent>(attacker);
	int cost = getActionCost(typeOfAction);
	if (attackerBattle.AP < cost) {
		spdlog::get("combat")->warn("Not enough AP to do this action");
		attackerBattle.battleState = BattleState::WAITING_FOR_INPUT;
		return;
	}
	attackerBattle.AP -= cost;

	if (typeOfAction == BattleAction::HEAL and attackerInventory.numberOfHealthPotions > 0) {
		attackerInventory.numberOfHealthPotions -= 1;
	} else if (typeOfAction == BattleAction::HEAL and attackerInventory.numberOfHealthPotions == 0) {
		spdlog::get("combat")->warn("No health potions left!");
		attackerBattle.battleState = BattleState::WAITING_FOR_INPUT;
		return;
	}
	if (typeOfAction == BattleAction::ULTIMATE_ATTACK and attackerBattle.numberOfUltimateAttacksUsed >= 1) {
		spdlog::get("combat")->warn("No ultimate attacks left!");
		attackerBattle.battleState = BattleState::WAITING_FOR_INPUT;
		return;
	}
	switch (typeOfAction) {

	case BattleAction::LIGHT_ATTACK: {
		auto &attackerStats = manager.getComponent<StatsComponent>(attacker);
		auto &attackerWeapon = manager.getComponent<WeaponComponent>(attacker);
		float damage = getDamageWithScaling(attackerStats, attackerWeapon, 5);
		auto health = manager.getComponent<StatsComponent>(defender).health;
		manager.getComponent<StatsComponent>(defender).health = std::max(0.0f, health - damage);
		break;
	}
	case BattleAction::HEAVY_ATTACK: {

		auto &attackerStats = manager.getComponent<StatsComponent>(attacker);
		auto &attackerWeapon = manager.getComponent<WeaponComponent>(attacker);
		float damage = getDamageWithScaling(attackerStats, attackerWeapon, 10);
		auto health = manager.getComponent<StatsComponent>(defender).health;
		manager.getComponent<StatsComponent>(defender).health = std::max(0.0f, health - damage);
		break;
	}

	case BattleAction::ULTIMATE_ATTACK: {

		auto &attackerStats = manager.getComponent<StatsComponent>(attacker);
		auto &attackerWeapon = manager.getComponent<WeaponComponent>(attacker);
		float damage = getDamageWithScaling(attackerStats, attackerWeapon, 15);
		auto health = manager.getComponent<StatsComponent>(defender).health;
		manager.getComponent<StatsComponent>(defender).health = std::max(0.0f, health - damage);
		manager.getComponent<BattleComponent>(attacker).numberOfUltimateAttacksUsed += 1;
		break;
	}
	case BattleAction::HEAL: {

		this->takeHealAction(attacker);
		break;
	}
	case BattleAction::REST: {

		this->restoreAP(attacker);
		break;
	}
	}
	spdlog::get("combat")->info(
	    "Entity {} attacks Entity {} with {}!", static_cast<int>(manager.getEntityTag(attacker).value()),
	    static_cast<int>(manager.getEntityTag(defender).value()), static_cast<int>(typeOfAction));
	spdlog::get("combat")->debug(
	    "Entity {} has {} HP and {} AP left!", static_cast<int>(manager.getEntityTag(defender).value()),
	    manager.getComponent<StatsComponent>(defender).health, manager.getComponent<BattleComponent>(defender).AP);
	spdlog::get("combat")->debug(
	    "Entity {} has {} HP and {} AP left!", static_cast<int>(manager.getEntityTag(attacker).value()),
	    manager.getComponent<StatsComponent>(attacker).health, manager.getComponent<BattleComponent>(attacker).AP);
}

void CombatSystem::takeHealAction(EntityID healer)
{
	StatsComponent &statsComponent = manager.getComponent<StatsComponent>(healer);
	statsComponent.health = std::min(100.0f, statsComponent.health + 10);
}

void CombatSystem::restoreAP(EntityID restorator)
{
	BattleComponent &battleComponent = manager.getComponent<BattleComponent>(restorator);
	battleComponent.AP += 2;
}

bool CombatSystem::handleActionDelay(BattleComponent battle)
{
	float dt = clock.restart().asSeconds();

	battle.actionTimer += dt;

	if (battle.actionTimer >= battle.actionDelay) {
		battle.actionTimer = 0.0f;
		return true;
	}

	return false;
}
BattleState CombatSystem::checkDeathCondition(EntityID defender)
{
	float health = manager.getComponent<StatsComponent>(defender).health;
	if (health > 0) {

		return BattleState::NEXT_ROUND;
	}
	if (health <= 0) {
		return BattleState::VICTORY;
	}
	throw std::runtime_error("Invalid health value");
}
void CombatSystem::passTurn(EntityID &currentEntity, int currentTurnIndex, const std::vector<EntityID> participants)
{

	manager.getComponent<BattleComponent>(currentEntity).isActiveTurn = false;
	EntityID nextId = this->getAttacker(currentTurnIndex, participants);
	auto &nextBattleComponent = manager.getComponent<BattleComponent>(nextId);
	nextBattleComponent.battleState = BattleState::TURN_START;
	nextBattleComponent.isActiveTurn = true;
	// spdlog::get("combat")->debug("Passing turn from entity {} to {}", manager.getEntityTag(currentEntity).value(),
	//                            manager.getEntityTag(nextId).value());
}
EntityID CombatSystem::getAttacker(int currentTurnIndex, const std::vector<EntityID> participants)
{
	if (participants.size() == 0) {
		throw std::runtime_error("No participants in battle");
	}
	return participants[currentTurnIndex % participants.size()];
}
void CombatSystem::cleanUpBattle(EntityID battleManagerId, EntityID winningEntity)
{
	auto &bmc = manager.getComponent<BattleManagerComponent>(battleManagerId);

	bool playerWon = manager.getEntityTag(winningEntity).value() == EntityTag::PLAYER;
	spdlog::get("combat")->debug("Has the player won? {}", playerWon);
	// Set health back to max and remove battle component from all entities in battle
	for (EntityID entity : bmc.participants) {
		manager.removeComponentFromEntity<BattleComponent>(entity);
		auto &stats = manager.getComponent<StatsComponent>(entity);
		stats.health = stats.maxHealth;
		if (entity == winningEntity) {
			if (playerWon) {
				stats.experienceLevel += 1;
				stats.numberOfFightsWon += 1;
			}
		}
	}
	// FIX ME: delete battleManagerEntity, needs to be implemented
	// manager.(battleManagerId);
	manager.removeComponentFromEntity<BattleManagerComponent>(battleManagerId);
	spdlog::get("combat")->debug("Cleanup is done");
}

float CombatSystem::getDamageWithScaling(const StatsComponent &statsComponent, const WeaponComponent &weaponComponent,
                                         float baseAttackDamage)
{
	if (weaponComponent.weaponType == WeaponType::MELEE) {
		return baseAttackDamage + statsComponent.strength * getMultiplicatorFromScalingFactor(weaponComponent);
	}
	if (weaponComponent.weaponType == WeaponType::RANGE) {
		return baseAttackDamage + statsComponent.dexterity * getMultiplicatorFromScalingFactor(weaponComponent);
	}
	throw std::runtime_error("Invalid weapon type");
}

float CombatSystem::getMultiplicatorFromScalingFactor(const WeaponComponent &weaponComponent)
{
	switch (weaponComponent.scalingFactor) {

	case ScalingFactor::A:
		return 3.0f;

	case ScalingFactor::B:
		return 2.0f;

	case ScalingFactor::C:
		return 1.0f;

	default:
		throw std::runtime_error("Invalid scaling factor");
	}
}

int CombatSystem::getActionCost(BattleAction action)
{
	switch (action) {
	case BattleAction::LIGHT_ATTACK:
		return 1;
	case BattleAction::HEAVY_ATTACK:
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
