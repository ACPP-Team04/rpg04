#pragma once
#include "Abstract/Combat/Systems/AISystem.hpp"
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/StatsComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
#include <Abstract/Combat/Components/BattleManagerComponent.hpp>
#include <SFML/Graphics.hpp>

class CombatSystem : public System {
  public:
	CombatSystem(ArchetypeManager &manager, AISystem &aiSystem) : System(manager), aiSystem(aiSystem) {};

	void update() override;

	void executeBattleAction(EntityID attacker, EntityID defender, BattleAction typeOfAction);

	void takeHealAction(EntityID healer, int faith, int maxHealth);

	void restoreAP(EntityID restorator);

	bool handleActionDelay(BattleComponent &battle);

	BattleState checkDeathCondition(EntityID defender, EntityID attacker);

	void passTurn(EntityID &currentEntity, BattleManagerComponent &bmc);

	EntityID getAttacker(BattleManagerComponent &bmc);

	static int getActionCost(BattleAction action);

	sf::Clock clock;

	void cleanUpBattle(EntityID battleManagerId, EntityID winningEntity, BattleState battleState);
	static bool validateAction(BattleAction action, int AP, int numberOfUltimateAttacksUsed, int numberOfHealsUsed);

  private:
	float getDamageWithScaling(const StatsComponent &statsComponent, const WeaponComponent &weaponComponent,
	                           BattleAction action);
	float getMultiplicatorFromScalingFactor(const StatsComponent stats, const WeaponComponent &weaponComponent);
	AISystem &aiSystem;
};
