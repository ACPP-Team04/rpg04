
#include "Abstract/Overwordl/SwitchBattleModeSystem.hpp"

#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
#include <Abstract/Overwordl/Components/InputComponent.hpp>
#include <Abstract/Overwordl/Components/InventoryComponent.hpp>
#include <Abstract/Overwordl/Components/MovementComponent.hpp>
#include <Abstract/Overwordl/Components/START_EQUIPMENT_COMPONENT.hpp>
#include <Abstract/Overwordl/Components/TransformComponent.hpp>
#include <spdlog/spdlog.h>

SwitchBattleModeSystem::SwitchBattleModeSystem(ArchetypeManager &manager) : System(manager) {}

void SwitchBattleModeSystem::update()
{
	std::optional<EntityID> interActorId = std::nullopt;
	InteractionComponent *icomp = nullptr;

	WorldUtils::viewInCurrentLayer<InteractionComponent>(manager, [&](auto entity, InteractionComponent &component) {
		if (!component.isActive) {
			return;
		}
		if (component.action == INTERACTION_ACTION::START_BATTLE) {
			interActorId = entity;
			icomp = &component;
		}
	});

	if (!interActorId.has_value() || icomp == nullptr) {

		return;
	}
	EntityID player = WorldUtils::getPlayer(manager).value();
	EntityID initialEnemyId = interActorId.value();
	icomp->isActive = false;

	if (manager.hasComponent<BattleComponent>(player))
		return;
	std::vector<EntityID> participantsList;
	participantsList.push_back(player);
	auto enemyList = getEnemiesInRatio(manager.getComponent<TransformComponent>(initialEnemyId).position, 50.0f);
	participantsList.insert(participantsList.end(), enemyList.begin(), enemyList.end());

	EntityID bManager = this->manager.createEntity<BattleManagerComponent, PartOfLayerComponent>();
	WorldComponent *world = WorldUtils::getWorld(manager);
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
		component.participants = {player, initialEnemyId};
		found = true;
	});

	prepareForBattle(participantsList, player);
	this->manager.getComponent<BattleManagerComponent>(bManager).participants = participantsList;

	for (const auto &participant : participantsList) {
		this->manager.getComponent<BattleComponent>(participant).battleManagerId = battleManagerId;
		auto inventory = this->manager.getComponent<InventoryComponent>(participant);
		auto weapon = inventory.getEquippedItem(ITEM_TYPE::WEAPON);
		if (!this->manager.hasComponent<BattleComponent>(participant)) {
			throw std::runtime_error("Batteling entity does not have a battle component");
		}
		if (!this->manager.hasComponent<StatsComponent>(participant)) {
			throw std::runtime_error("Batteling entity does not have a stat component");
		}
		std::cout << "Health for entity " << participant.getId() << ": "
		          << this->manager.getComponent<StatsComponent>(participant).health << std::endl;
		if (!this->manager.hasComponent<InventoryComponent>(participant)) {
			throw std::runtime_error("Batteling entity does not have a inventory component");
		}
	}
	spdlog::get("combat")->info("Switched to battle mode");
}

std::vector<EntityID> SwitchBattleModeSystem::getEnemiesInRatio(const sf::Vector2f center, float radius)
{
	std::vector<EntityID> enemiesIdList;
	this->manager.view<InventoryComponent, TransformComponent>().each(
	    [&](auto entityId, auto &eqComponent, auto &transformComponent) {
		    if (!WorldUtils::isPartOfCurrentLayer(this->manager, entityId)) {
			    return;
		    }
		    if (SwitchBattleModeSystem::getSquaredDistance(center, transformComponent.position) <= radius * radius) {
			    enemiesIdList.push_back(entityId);
		    }
	    });
	spdlog::get("combat")->info("Found {} enemies in the action radius", enemiesIdList.size());
	return enemiesIdList;
}

void SwitchBattleModeSystem::prepareForBattle(const std::vector<EntityID> &participants, EntityID playerId)
{
	for (const auto &participant : participants) {
		if (participant == playerId) {
			this->manager.removeComponentFromEntity<InputComponent>(participant);
		}
		this->manager.addComponentToEntity<BattleComponent>(participant);
	}
}
