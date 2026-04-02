#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/InventoryComponent.hpp"
#include "Implementation/Components/StatsComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"

// Check if there are entities in battleState if yes, deal with their plays
void CombatSystem::update()
{
	auto view = manager.view<BattleManagerComponent>();

	if (view.archetypes.size() == 0) {
		std::cout << "No BattleManagerComponent found" << std::endl;
		return;
	} else {
		view.each([&](EntityID battleId, BattleManagerComponent &bmc) {
			EntityID currentAttacker = this->getAttacker(bmc.currentTurnIndex, bmc.participants);
			if (bmc.isBattleOver) {
				cleanUpBattle(battleId, currentAttacker);
				return;
			}
			EntityID currentAttacker = this->getAttacker(bmc.currentTurnIndex, bmc.participants);
			BattleComponent &battle = manager.getComponent<BattleComponent>(currentAttacker);
			battle.isActiveTurn = true;
			switch (battle.battleState) {
			case BattleState::TURN_START:
				battle.battleState = BattleState::WAITING_FOR_INPUT;
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
			case BattleState::CHECK_DEATH:
				battle.battleState = this->checkDeathCondition(battle.target);
				break;
			case BattleState::NEXT_ROUND:
				bmc.currentTurnIndex++;
				this->passTurn(currentAttacker, bmc.currentTurnIndex, bmc.participants);
				break;
			case BattleState::VICTORY:
				// remove battle component from all entities in battle and set isActiveTurn to false
				std::cout << "Victory for player" << std::endl;
				bmc.isBattleOver = true;
				break;
			case BattleState::DEFEAT:
				// remove battle component from all entities in battle and set isActiveTurn to false
				std::cout << "Defeat! for player" << std::endl;
				bmc.isBattleOver = true;
				break;
			}
		});
	}
}

// add the type of attack as parameter
void CombatSystem::executeBattleAction(EntityID attacker, EntityID defender, BattleAction typeOfAction)
{
	auto &attackerBattle = manager.getComponent<BattleComponent>(attacker);
	auto &attackerInventory = manager.getComponent<InventoryComponent>(attacker);
	int cost = getActionCost(typeOfAction);
	if (attackerBattle.AP < cost) {
		std::cout << "Not enough AP to do this action" << std::endl;
		attackerBattle.battleState = BattleState::WAITING_FOR_INPUT;
		return;
	}
	attackerBattle.AP -= cost;

	if (typeOfAction == BattleAction::HEAL and attackerInventory.numberOfHealthPotions > 0) {
		attackerInventory.numberOfHealthPotions -= 1;
	} else if (typeOfAction == BattleAction::HEAL and attackerInventory.numberOfHealthPotions == 0) {
		std::cout << "No health potions left!" << std::endl;
		attackerBattle.battleState = BattleState::WAITING_FOR_INPUT;
		return;
	}
	if (typeOfAction == BattleAction::ULTIMATE_ATTACK and attackerBattle.numberOfUltimateAttacksUsed >= 1) {
		std::cout << "No ultimate attacks left!" << std::endl;
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
	// later add this code to leave time for fighting animations, etc. For now, we just return true to immediately go to
	// the next state
	/*
	float dt = clock.restart().asSeconds();

	battle.actionTimer += dt;

	if (battle.actionTimer >= battle.actionDelay) {
	    battle.actionTimer = 0.0f;
	    return true;
	}

	return false;
	*/
	return true;
}
BattleState CombatSystem::checkDeathCondition(EntityID defender)
{
	float health = manager.getComponent<StatsComponent>(defender).health;
	if (health > 0) {

		return BattleState::NEXT_ROUND;
	}
	// need to add a tag here to check if the entity is player or enemy to set the correct battle state
	if (health <= 0) {
		return BattleState::VICTORY;
	}
	throw std::runtime_error("Invalid health value");
}
void CombatSystem::passTurn(EntityID &currentEntity, int currentTurnIndex, const std::vector<EntityID> participants)
{
	std::cout << "Passing turn from entity " << currentEntity.getId() << std::endl;
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
void CombatSystem::cleanUpBattle(EntityID battleManagerId, EntityID winningEntity)
{
	auto &bmc = manager.getComponent<BattleManagerComponent>(battleManagerId);

	bool playerWon = manager.getEntityTag(winningEntity) == EntityTag::PLAYER;

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
