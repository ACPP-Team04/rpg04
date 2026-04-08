
#include "Abstract/Overwordl/SwitchBattleModeSystem.hpp"

#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"

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

	this->manager.addComponentToEntity<BattleComponent>(*player);
	this->manager.addComponentToEntity<BattleComponent>(*interActor);

	this->manager.createEntity<BattleManagerComponent>();

	EntityID *battleManager = nullptr;
	this->manager.view<BattleManagerComponent>().each([&](auto &entity, BattleManagerComponent &component) {
		battleManager = &entity;
		component.participants = {*player, *interActor};
	});
	this->manager.getComponent<BattleComponent>(*player).battleManagerId = *battleManager;
	this->manager.getComponent<BattleComponent>(*interActor).battleManagerId = *battleManager;
	this->manager.addComponentToEntity<WeaponComponent>(*player);
	this->manager.addComponentToEntity<WeaponComponent>(*interActor);
}