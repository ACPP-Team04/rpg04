
#include "Abstract/Overwordl/SwitchBattleModeSystem.hpp"

#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
#include <Abstract/Overwordl/Components/InputComponent.hpp>
#include <Abstract/Overwordl/Components/MovementComponent.hpp>
#include <Abstract/Overwordl/Components/StateComponent.hpp>
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
	if (!manager.hasComponent<CharacterComponent>(player)) {
		return;
	}

	EntityID initialEnemyId = interActorId.value();
	icomp->isActive = false;

	if (manager.hasComponent<BattleComponent>(player))
		return;

	std::vector<EntityID> participantsList;
	participantsList.push_back(player);
	if (!manager.hasComponent<CharacterComponent>(player)) {
		spdlog::error("Player entity does not have a CharacterComponent, cannot start battle");
		throw std::runtime_error("Player entity does not have a CharacterComponent, cannot start battle");
	}
	auto &characterP = manager.getComponent<CharacterComponent>(player);

	if (characterP.equipedCompanion != 0) {
		auto companionId = characterP.equipedCompanion;
		EntityID companionIdEntity = EntityID::fromExistingId(companionId);
		if (!manager.hasComponent<PartOfLayerComponent>(companionIdEntity)) {
			manager.addComponentToEntity<PartOfLayerComponent>(companionIdEntity);
		}
		auto world = WorldUtils::getWorld(manager);
		auto &compLayer = manager.getComponent<PartOfLayerComponent>(companionId);
		compLayer.groupId = world->currentGroup;

		auto &playerTransform = manager.getComponent<TransformComponent>(player);
		auto &compTransform = manager.getComponent<TransformComponent>(companionId);

		auto &enemyTransform = manager.getComponent<TransformComponent>(initialEnemyId);

		sf::Vector2f difference = playerTransform.position - enemyTransform.position;
		float distance = std::sqrt((difference.x * difference.x) + (difference.y * difference.y));

		sf::Vector2f direction(0.0f, 0.0f);
		if (distance > 0.0f) {
			direction = sf::Vector2f(difference.x / distance, difference.y / distance);
		} else {
			direction = sf::Vector2f(-1.0f, 0.0f);
		}

		float spawnDistance = 30.0f;
		compTransform.position = playerTransform.position + (direction * spawnDistance);
		participantsList.push_back(companionIdEntity);
		spdlog::info("Added companion with id {} to battle", companionIdEntity.getId());
	}
	preparePlayerPartyForBattle(participantsList, player);
	auto enemyList = getEnemiesInRatio(
	    initialEnemyId, manager.getComponent<TransformComponent>(initialEnemyId).position, 65.0f, participantsList);
	prepareEnemiesForBattle(enemyList);
	participantsList.insert(participantsList.end(), enemyList.begin(), enemyList.end());

	EntityID bManager = this->manager.createEntity<BattleManagerComponent, PartOfLayerComponent>();
	WorldComponent *world = WorldUtils::getWorld(manager);
	if (world == nullptr) {
		return;
	}
	spdlog::info("Battlemanager created with id: {}", bManager.getId());
	this->manager.getComponent<PartOfLayerComponent>(bManager).groupId = world->currentGroup;

	this->manager.getComponent<BattleManagerComponent>(bManager).participants = participantsList;

	for (const auto &participant : participantsList) {
		if (!this->manager.hasComponent<BattleComponent>(participant)) {
			throw std::runtime_error("Batteling entity does not have a battle component");
		}
		if (manager.getComponent<CharacterComponent>(participant).equipedWeapon == 0) {
			throw std::runtime_error("Batteling entity should not have an equiped weapon");
		}
		manager.getComponent<BattleComponent>(participant).battleManagerId = bManager;
	}
	audioSystem.switchMusic("combat", true);
	spdlog::get("combat")->info("Turn Start! Participant count: {}", participantsList.size());
	spdlog::get("combat")->info("Switched to battle mode");
}

std::vector<EntityID> SwitchBattleModeSystem::getEnemiesInRatio(const EntityID &initialEnemy, const sf::Vector2f center,
                                                                float radius, const std::vector<EntityID> &playerParty)
{
	std::vector<EntityID> enemiesIdList;
	enemiesIdList.push_back(initialEnemy);
	WorldUtils::viewInCurrentLayer<CharacterComponent, TransformComponent>(
	    this->manager, [&](auto entityId, CharacterComponent &characterComponent, auto &transformComponent) {
		    if (std::find(playerParty.begin(), playerParty.end(), entityId) != playerParty.end()) {
			    return;
		    }
		    if (!characterComponent.fightable) {
			    return;
		    }
		    if (entityId == initialEnemy) {
			    return;
		    }
		    auto squaredDistance = SwitchBattleModeSystem::getSquaredDistance(center, transformComponent.position);
		    if (squaredDistance <= 0) {
			    return;
		    }
		    if (squaredDistance <= radius * radius) {
			    enemiesIdList.push_back(entityId);
		    } else {
			    spdlog::info("Entity with id {} is not within the battle radius, has {}", entityId.getId(),
			                 squaredDistance);
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
			this->manager.getComponent<StateComponent>(participant).setState(ENTITY_ANIMATIONS_STATE::IDLE);
		}
		if (!manager.hasComponent<CharacterComponent>(participant)) {
			throw std::runtime_error("Player party entity does not have a CharacterComponent, cannot start battle");
		}
		if (!manager.hasComponent<StateComponent>(participant)) {
			throw std::runtime_error("Player party entity does not have a StateComponent, cannot start battle");
		}
		manager.addComponentToEntity<BattleComponent>(participant);
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
