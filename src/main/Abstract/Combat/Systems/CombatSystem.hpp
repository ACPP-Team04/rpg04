#pragma once
#include "Abstract/Combat/Systems/AISystem.hpp"
#include "Abstract/ECS/Archetype/ArchetypeManager.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/ECS/System/System.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"

#include <Abstract/Audio/AudioManager.hpp>
#include <Abstract/Audio/AudioSystem.hpp>
#include <Abstract/Combat/Components/BattleManagerComponent.hpp>
#include <SFML/Graphics.hpp>

class CombatSystem : public System {
  public:
	CombatSystem(ArchetypeManager &manager, AISystem &aiSystem, AudioSystem &audioSystem)
	    : System(manager), aiSystem(aiSystem), audioSystem(audioSystem) {};

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

	void cleanUpBattle(EntityID battleManagerId, BATTLE_FACTION winningBattleFaction, BattleState battleState);
	static bool validateAction(BattleAction action, const BattleComponent &battle);

  private:
	float getDamageWithScaling(const StatsComponent &statsComponent, const WeaponComponent &weaponComponent,
	                           BattleAction action);
	float getMultiplicatorFromScalingFactor(const StatsComponent stats, const WeaponComponent &weaponComponent);
	void setupKineticLunge(const EntityID &attacker, const EntityID &defender);
	void moveCompanionToInventory(const EntityID &entityId, const int inventoryWorldId);
	void processBattleTick(const EntityID &battleId, BattleManagerComponent &bmc);
	void handleEntityOfWinningFaction(const EntityID &entity, const EntityID &playerId);
	AISystem &aiSystem;
	AudioSystem &audioSystem;
};
