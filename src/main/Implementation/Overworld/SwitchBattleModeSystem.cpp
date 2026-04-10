
#include "Abstract/Overwordl/SwitchBattleModeSystem.hpp"

#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
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
	WorldUtils::viewInCurrentLayer<InteractionComponent>(manager, [&](auto &entity, InteractionComponent &component) {
		if (!component.isActive) {
			return;
		}
		if (component.action == INTERACTION_ACTION::START_BATTLE) {
			interActor = &entity;
			icomp = &component;
		}
	});

	if (icomp == nullptr)
		return;
	EntityID player = WorldUtils::getPlayer(manager).value();

	icomp->isActive = false;

	if (manager.hasComponent<BattleComponent>(player))
		return;

	this->manager.removeComponentFromEntity<MovementComponent>(player);
	this->manager.addComponentToEntity<BattleComponent>(player);
	this->manager.addComponentToEntity<BattleComponent>(*interActor);

	EntityID bManager =this->manager.createEntity<BattleManagerComponent, PartOfLayerComponent>(EntityTag::BATTLEMANAGER);
	WorldComponent *world =WorldUtils::getWorld(manager);
	if (world == nullptr) {
		return;
	}
	this->manager.getComponent<PartOfLayerComponent>(bManager).layer = world->currentLayer;
	this->manager.getComponent<PartOfLayerComponent>(bManager).level = world->currentLevel;

	EntityID battleManagerId = 0;
	bool found = false;
	this->manager.view<BattleManagerComponent>().each([&](auto entity, BattleManagerComponent &component) {
		if (!WorldUtils::isPartOfCurrentLayer(this->manager, entity)) {
			return;
		}
		battleManagerId = entity;
		component.participants = {*player, *interActor};
		found = true;
	});

	this->manager.getComponent<BattleManagerComponent>(bManager).participants.push_back(player);
	this->manager.getComponent<BattleManagerComponent>(bManager).participants.push_back(*interActor);

	this->manager.getComponent<BattleComponent>(player).battleManagerId = battleManagerId;
	this->manager.getComponent<BattleComponent>(*interActor).battleManagerId = battleManagerId;

	for (auto& entity:{player, *interActor}) {
		if (!this->manager.hasComponent<BattleComponent>(entity)) {
			throw std::runtime_error("Batteling entity does not have a battle component");
		}
		if (!this->manager.hasComponent<StatsComponent>(entity)) {
			throw std::runtime_error("Batteling entity does not have a stat component");
		}
		if (!this->manager.hasComponent<InventoryComponent>(entity)) {
			throw std::runtime_error("Batteling entity does not have a inventory component");
		}


	}
	spdlog::get("combat")->info("Switched to battle mode");
	//
}
