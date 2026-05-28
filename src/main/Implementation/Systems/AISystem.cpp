#include "Abstract/Combat/Systems/AISystem.hpp"
#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemHealstatsComponent.hpp"
#include "Implementation/Components/BattleComponent.hpp"

#include "Implementation/Components/StatsComponent.hpp"
#include <Abstract/Combat/Components/DeathComponent.hpp>
#include <random>

AISystem::AISystem(ArchetypeManager &manager) : System(manager) {};

std::optional<EntityID> AISystem::selectTarget(EntityID aiId, const std::vector<EntityID> &participants)
{
	auto &aiBattleComp = manager.getComponent<BattleComponent>(aiId);

	std::vector<EntityID> validTargets;

	for (EntityID p : participants) {
		if (aiId == p || manager.hasComponent<DeathComponent>(p)) {
			continue;
		}

		if (manager.hasComponent<BattleComponent>(p)) {
			auto &targetBattleComp = manager.getComponent<BattleComponent>(p);

			if (targetBattleComp.faction != aiBattleComp.faction) {
				validTargets.push_back(p);
			}
		}
	}

	if (validTargets.empty()) {
		return std::nullopt;
	}

	int randomIndex = rand() % validTargets.size();
	return validTargets[randomIndex];
}
void AISystem::executeAILogic(EntityID aiId, std::vector<EntityID> participants)
{
	BattleComponent &aiBattle = manager.getComponent<BattleComponent>(aiId);
	StatsComponent &aiStats = manager.getComponent<CharacterComponent>(aiId).stats;

	auto targetOpt = selectTarget(aiId, participants);
	if (!targetOpt.has_value()) {
		throw std::runtime_error("No valid target found for AI entity " + std::to_string(aiId.getId()));
	}
	aiBattle.target = targetOpt.value();

	if (aiStats.health < 0.2 * aiStats.getStat(MAX_HEALTH) && aiBattle.numberOfHealsUsed < 2
	    && aiBattle.AP >= CombatSystem::getActionCost(BattleAction::HEAL)) {
		aiBattle.selectedAction = BattleAction::HEAL;
	} else if (aiBattle.AP >= CombatSystem::getActionCost(BattleAction::HEAVY_ATTACK)) {
		aiBattle.selectedAction = BattleAction::HEAVY_ATTACK;
	} else if (aiBattle.AP >= CombatSystem::getActionCost(BattleAction::LIGHT_ATTACK)) {
		aiBattle.selectedAction = BattleAction::LIGHT_ATTACK;
	} else if (aiBattle.numberOfUltimateAttacksUsed < 1) {
		aiBattle.selectedAction = BattleAction::ULTIMATE_ATTACK;
	} else {
		aiBattle.selectedAction = BattleAction::REST;
	}
	aiBattle.battleState = BattleState::SELECTED_ACTION;
}
void AISystem::update() {};
