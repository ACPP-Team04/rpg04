#pragma once
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/InventoryComponent.hpp"
#include "Implementation/Components/StatsComponent.hpp"

class AISystem : public System {
  public:
	AISystem(ArchetypeManager &manager);
	void executeAILogic(EntityID aiId, std::vector<EntityID> participants);
	void update();
};
