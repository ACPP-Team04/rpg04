#pragma once
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/StatsComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
#include <SFML/Graphics.hpp>

class CombatSystem : public System {
  public:
	CombatSystem(ArchetypeManager &manager) : System(manager) {};

	void update() override;

	void executeBattleAction(EntityID attacker, EntityID defender, BattleAction typeOfAction);

	void takeHealAction(EntityID healer);

	void restoreAP(EntityID restorator);

	bool handleActionDelay(BattleComponent battle);

	BattleState checkDeathCondition(EntityID defender);

	void passTurn(EntityID &currentEntity, int currentTurnIndex, const std::vector<EntityID> participants);

	EntityID getAttacker(int currentTurnIndex, const std::vector<EntityID> participants);

	static int getActionCost(BattleAction action);

	sf::Clock clock;

	void cleanUpBattle(EntityID battleManagerId, EntityID winningEntity);

  private:
	float getDamageWithScaling(const StatsComponent &statsComponent, const WeaponComponent &weaponComponent,
	                           float baseAttackDamage);
	float getMultiplicatorFromScalingFactor(const WeaponComponent &weaponComponent);
};
