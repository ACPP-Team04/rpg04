
#include "Abstract/Overwordl/SwitchBattleModeSystem.hpp"

#include "Abstract/Combat/Components/BattleManagerComponent.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/ItemComponent.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "Implementation/Components/BattleComponent.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
#include <Abstract/Exception/MissingComponentException.hpp>
#include <Abstract/Exception/PlayerComponentIncompleteException.hpp>
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

	WorldUtils::viewInCurrentLayer<InteractionComponent>(
	    manager, [this, &interActorId, &icomp](auto entity, InteractionComponent &component) {
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
		throw PlayerComponentIncompleteException(
		    "Player entity does not have a CharacterComponent, cannot start battle");
	}

	if (const auto &characterP = manager.getComponent<CharacterComponent>(player); characterP.equipedCompanion != 0) {
		auto companionId = characterP.equipedCompanion;
		EntityID companionIdEntity = EntityID::fromExistingId(companionId);
		if (!manager.hasComponent<PartOfLayerComponent>(companionIdEntity)) {
			manager.addComponentToEntity<PartOfLayerComponent>(companionIdEntity);
		}
		auto world = WorldUtils::getWorld(manager);
		auto &compLayer = manager.getComponent<PartOfLayerComponent>(companionId);
		compLayer.groupId = world->currentGroup;

		const auto &playerTransform = manager.getComponent<TransformComponent>(player);
		auto &compTransform = manager.getComponent<TransformComponent>(companionId);

		const auto &enemyTransform = manager.getComponent<TransformComponent>(initialEnemyId);

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
	int reward = determineXpRewardForPlayer(enemyList.size());
	manager.getComponent<BattleManagerComponent>(bManager).playerXpReward = reward;
	const WorldComponent *world = WorldUtils::getWorld(manager);
	if (world == nullptr) {
		return;
	}
	spdlog::info("Battlemanager created with id: {}", bManager.getId());
	this->manager.getComponent<PartOfLayerComponent>(bManager).groupId = world->currentGroup;

	this->manager.getComponent<BattleManagerComponent>(bManager).participants = participantsList;

	for (const auto &participant : participantsList) {
		if (!this->manager.hasComponent<BattleComponent>(participant)) {
			throw MissingComponentException("Battling entity does not have a battle component");
		}

		if (auto &character = manager.getComponent<CharacterComponent>(participant); character.equipedWeapon == 0) {
			EntityID fist = manager.createEntity<ItemComponent, PartOfLayerComponent>();
			auto &fistItem = manager.getComponent<ItemComponent>(fist);
			fistItem.itemType = ITEM_TYPE::WEAPON;
			fistItem.name = "FIST";
			manager.getComponent<PartOfLayerComponent>(fist).groupId = character.inventory.inventoryWorldId;
			character.equipedWeapon = fist.getId();
			spdlog::info("Equipped fallback fists for battle participant {}", participant.getId());
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
	    this->manager, [this, &playerParty, &enemiesIdList, &initialEnemy, &center, &radius](
	                       auto entityId, const CharacterComponent &characterComponent, auto &transformComponent) {
		    if (!isValidAdditionalEnemy(entityId, characterComponent, playerParty, initialEnemy)) {
			    return;
		    }
		    auto squaredDistance = SwitchBattleModeSystem::getSquaredDistance(center, transformComponent.position);
		    if (squaredDistance <= 0) {
			    return;
		    }
		    if (squaredDistance <= radius * radius) {
			    spdlog::debug("Entity with id {} is within the battle radius, has {}", entityId.getId(),
			                  squaredDistance);
			    enemiesIdList.push_back(entityId);
		    } else {
			    spdlog::info("Entity with id {} is not within the battle radius, has {}", entityId.getId(),
			                 squaredDistance);
		    }
	    });
	spdlog::get("combat")->info("Found {} enemies in the action radius", enemiesIdList.size());
	return enemiesIdList;
}

bool SwitchBattleModeSystem::isValidAdditionalEnemy(EntityID entityId, const CharacterComponent &charComp,
                                                    const std::vector<EntityID> &playerParty,
                                                    EntityID initialEnemy) const
{
	if (std::ranges::find(playerParty, entityId) != playerParty.end()) {
		return false;
	}
	if (!charComp.fightable) {
		return false;
	}
	if (entityId == initialEnemy) {
		return false;
	}
	if (charComp.stats.health <= 0) {
		return false;
	}
	return true;
}

void SwitchBattleModeSystem::preparePlayerPartyForBattle(const std::vector<EntityID> &participants, EntityID playerId)
{
	for (const auto &participant : participants) {
		if (participant == playerId) {
			this->manager.removeComponentFromEntity<InputComponent>(participant);
			this->manager.getComponent<StateComponent>(participant).setState(ENTITY_ANIMATIONS_STATE::IDLE);
		}
		if (!manager.hasComponent<CharacterComponent>(participant)) {
			throw MissingComponentException(
			    "Player party entity does not have a CharacterComponent, cannot start battle");
		}
		if (!manager.hasComponent<StateComponent>(participant)) {
			throw MissingComponentException("Player party entity does not have a StateComponent, cannot start battle");
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

int SwitchBattleModeSystem::determineXpRewardForPlayer(size_t numberOfEnemies) const
{
	const float BASE_XP_PER_ENEMY = 1.0f;
	float multiplier = 1.0f + (0.5f * (static_cast<float>(numberOfEnemies) - 1));

	float result = (static_cast<float>(numberOfEnemies) * BASE_XP_PER_ENEMY) * multiplier;

	return static_cast<int>(result);
}
