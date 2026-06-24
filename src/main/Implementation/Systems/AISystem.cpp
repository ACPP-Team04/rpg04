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
#include <Abstract/Exception/InvalidCombatTargetException.hpp>
#include <random>

AISystem::AISystem(ArchetypeManager &manager) : System(manager) {};

std::optional<EntityID> AISystem::selectTarget(EntityID aiId, const std::vector<EntityID> &participants)
{
	std::vector<EntityID> validTargets = this->getValidTargets(aiId, participants);

	if (validTargets.empty()) {
		return std::nullopt;
	}
	static thread_local std::mt19937 generator(std::random_device{}()); // NOSONAR
	std::uniform_int_distribution<std::size_t> distribution(0, validTargets.size() - 1);
	return validTargets[distribution(generator)];
}

std::vector<EntityID> AISystem::getValidTargets(EntityID aiId, const std::vector<EntityID> &participants)
{
	auto &aiBattleComp = manager.getComponent<BattleComponent>(aiId);
	std::vector<EntityID> validTargets;

	for (EntityID p : participants) {
		if (aiId == p || manager.hasComponent<DeathComponent>(p)) {
			continue;
		}
		if (manager.hasComponent<BattleComponent>(p)) {
			const auto &targetBattleComp = manager.getComponent<BattleComponent>(p);

			if (targetBattleComp.faction != aiBattleComp.faction) {
				validTargets.push_back(p);
			}
		}
	}

	return validTargets;
}

void AISystem::executeAILogic(EntityID aiId, const std::vector<EntityID> &participants)
{
	BattleComponent &aiBattle = manager.getComponent<BattleComponent>(aiId);
	const StatsComponent &aiStats = manager.getComponent<CharacterComponent>(aiId).stats;

	auto targetOpt = selectTarget(aiId, participants);
	if (!targetOpt.has_value()) {
		throw InvalidCombatTargetException(
		    std::format("No valid target found for AI entity {}", std::to_string(aiId.getId())));
	}
	aiBattle.target = targetOpt.value();

	using enum BattleAction;
	if (aiStats.health < 0.2 * aiStats.getStat(MAX_HEALTH) && aiBattle.numberOfHealsUsed < 2
	    && aiBattle.AP >= static_cast<int>(CombatSystem::getActionCost(HEAL))) {
		aiBattle.selectedAction = HEAL;
	} else if (aiBattle.AP >= static_cast<int>(CombatSystem::getActionCost(HEAVY_ATTACK))) {
		aiBattle.selectedAction = HEAVY_ATTACK;
	} else if (aiBattle.AP >= static_cast<int>(CombatSystem::getActionCost(LIGHT_ATTACK))) {
		aiBattle.selectedAction = LIGHT_ATTACK;
	} else if (aiBattle.numberOfUltimateAttacksUsed < 1) {
		aiBattle.selectedAction = ULTIMATE_ATTACK;
	} else {
		aiBattle.selectedAction = REST;
	}
	aiBattle.battleState = BattleState::SELECTED_ACTION;
}
void AISystem::update() {
	// Intentionally empty, as the AI logic is executed directly from the CombatSystem when it's the AI's turn
};
