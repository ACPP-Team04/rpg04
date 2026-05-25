
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
#include <Abstract/Overwordl/Components/NPC_COMPONENT.hpp>
#include <Abstract/Overwordl/Components/START_EQUIPMENT_COMPONENT.hpp>
#include <Abstract/Overwordl/Components/TransformComponent.hpp>
#include <spdlog/spdlog.h>

SwitchBattleModeSystem::SwitchBattleModeSystem(ArchetypeManager &manager, AudioSystem &audioSystem)
    : System(manager), audioSystem(audioSystem)
{
}

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
	auto &invP = manager.getComponent<InventoryComponent>(player);

	if (invP.hasEquippedItem(ITEM_TYPE::COLLECTABLE_COMPANION)) {
		auto companionId = invP.getEquippedItem(ITEM_TYPE::COLLECTABLE_COMPANION);
		if (!manager.hasComponent<PartOfLayerComponent>(companionId)) {
			manager.addComponentToEntity<PartOfLayerComponent>(companionId);
		}
		auto world = WorldUtils::getWorld(manager);
		auto &compLayer = manager.getComponent<PartOfLayerComponent>(companionId);
		compLayer.layer = world->currentLayer;
		compLayer.level = world->currentLevel;

		auto &playerTransform = manager.getComponent<TransformComponent>(player);
		auto &compTransform = manager.getComponent<TransformComponent>(companionId);

		compTransform.position = playerTransform.position + sf::Vector2f(-50.0f, 0.0f);
		participantsList.push_back(companionId);
		spdlog::info("Added companion with id {} to battle", companionId.getId());
	}
	preparePlayerPartyForBattle(participantsList, player);
	auto enemyList =
	    getEnemiesInRatio(manager.getComponent<TransformComponent>(initialEnemyId).position, 50.0f, participantsList);
	prepareEnemiesForBattle(enemyList);
	participantsList.insert(participantsList.end(), enemyList.begin(), enemyList.end());

	EntityID bManager = this->manager.createEntity<BattleManagerComponent, PartOfLayerComponent>();
	WorldComponent *world = WorldUtils::getWorld(manager);
	if (world == nullptr) {
		return;
	}
	this->manager.getComponent<PartOfLayerComponent>(bManager).layer = world->currentLayer;
	this->manager.getComponent<PartOfLayerComponent>(bManager).level = world->currentLevel;

	this->manager.getComponent<BattleManagerComponent>(bManager).participants = participantsList;

	for (const auto &participant : participantsList) {
		this->manager.getComponent<BattleComponent>(participant).battleManagerId = bManager;
		auto &inventory = this->manager.getComponent<InventoryComponent>(participant);
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
	audioSystem.switchMusic("combat", true);
	spdlog::get("combat")->info("Switched to battle mode");
}

std::vector<EntityID> SwitchBattleModeSystem::getEnemiesInRatio(const sf::Vector2f center, float radius,
                                                                const std::vector<EntityID> &playerParty)
{
	std::vector<EntityID> enemiesIdList;
	this->manager.view<InventoryComponent, TransformComponent>().each(
	    [&](auto entityId, auto &eqComponent, auto &transformComponent) {
		    if (std::find(playerParty.begin(), playerParty.end(), entityId) != playerParty.end()) {
			    return;
		    }
		    if (manager.hasComponent<NPC_Component>(entityId)) {
			    return;
		    }
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

void SwitchBattleModeSystem::preparePlayerPartyForBattle(const std::vector<EntityID> &participants, EntityID playerId)
{
	for (const auto &participant : participants) {
		if (participant == playerId) {
			this->manager.removeComponentFromEntity<InputComponent>(participant);
		}
		this->manager.addComponentToEntity<BattleComponent>(participant);
		auto &battleComp = this->manager.getComponent<BattleComponent>(participant);
		battleComp.faction = BATTLE_FACTION::PLAYER_PARTY;
		battleComp.controller = BATTLE_CONTROLLER::LOCAL_PLAYER;
	}
}

void SwitchBattleModeSystem::prepareEnemiesForBattle(const std::vector<EntityID> &enemies)
{
	for (const auto &participant : enemies) {
		this->manager.addComponentToEntity<BattleComponent>(participant);
		auto &battleComp = this->manager.getComponent<BattleComponent>(participant);
		battleComp.faction = BATTLE_FACTION::ENEMY;
		battleComp.controller = BATTLE_CONTROLLER::AI;
	}
}
