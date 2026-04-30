#include "Abstract/Combat/Systems/AISystem.hpp"
#include "Abstract/Combat/Systems/CombatSystem.hpp"
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemHealstatsComponent.hpp"
#include "Implementation/Components/BattleComponent.hpp"

#include "Implementation/Components/StatsComponent.hpp"

AISystem::AISystem(ArchetypeManager &manager) : System(manager) {};
void AISystem::executeAILogic(EntityID aiId, std::vector<EntityID> participants)
{
	BattleComponent &aiBattle = manager.getComponent<BattleComponent>(aiId);
	StatsComponent &aiStats = manager.getComponent<StatsComponent>(aiId);
	EntityID target;
	for (EntityID p : participants) {
		if (aiId != p) {
			target = p;
			break;
		}
	}

	aiBattle.target = target;

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
