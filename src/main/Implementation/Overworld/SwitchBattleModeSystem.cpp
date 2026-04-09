
#include "Abstract/Overwordl/SwitchBattleModeSystem.hpp"

#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
#include <Abstract/Overwordl/Components/InventoryComponent.hpp>
#include <Abstract/Overwordl/Components/MovementComponent.hpp>
#include <spdlog/spdlog.h>

SwitchBattleModeSystem::SwitchBattleModeSystem(ArchetypeManager &manager) : System(manager) {}

void SwitchBattleModeSystem::update()
{

	InteractionComponent *icomp = nullptr;
	EntityID *interActor = nullptr;
	this->manager.view<InteractionComponent>().each([&](auto &entity, InteractionComponent &component) {
		if (!component.isActive) {
			return;
		}
		if (component.action == INTERACTION_ACTION::START_BATTLE) {
			interActor = &entity;
			icomp = &component;
			return;
		}
	});

	if (icomp == nullptr)
		return;
	EntityID *player = nullptr;
	this->manager.view<PlayerComponent>().each([&](auto &entity, PlayerComponent &component) { player = &entity; });

	if (player == nullptr)
		return;
	icomp->isActive = false;

	if (manager.hasComponent<BattleComponent>(*player))
		return;

	this->manager.removeComponentFromEntity<MovementComponent>(*player);
	this->manager.addComponentToEntity<BattleComponent>(*player);
	this->manager.addComponentToEntity<BattleComponent>(*interActor);

	this->manager.createEntity<BattleManagerComponent>(EntityTag::BATTLEMANAGER);

	EntityID battleManagerId = 0;
	bool found = false;

	this->manager.view<BattleManagerComponent>().each([&](auto entity, BattleManagerComponent &component) {
		battleManagerId = entity;
		component.participants = {*player, *interActor};
		found = true;
	});

	if (found) {
		this->manager.getComponent<BattleComponent>(*player).battleManagerId = battleManagerId;
		this->manager.getComponent<BattleComponent>(*interActor).battleManagerId = battleManagerId;
	} else {
		spdlog::error("BattleManager not found in view!");
	}
	this->manager.addComponentToEntity<WeaponComponent>(*player);
	this->manager.addComponentToEntity<WeaponComponent>(*interActor);

	// DEBUG: Add stats component to enemy and player
	this->manager.addComponentToEntity<StatsComponent>(*player);
	this->manager.addComponentToEntity<StatsComponent>(*interActor);
	// DEBUG: Add inventory for enemy
	this->manager.addComponentToEntity<InventoryComponent>(*interActor);
	this->manager.getComponent<StatsComponent>(*interActor).health = 10;
	spdlog::get("combat")->info("Switched to battle mode");
	//
}
