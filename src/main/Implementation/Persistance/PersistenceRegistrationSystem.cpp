#include "Abstract/Persistance/PersistenceRegistrationSystem.hpp"
#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/DialogComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/IsLockedComponent.hpp"
#include "Abstract/Overwordl/Components/ItemComponent.hpp"
#include "Abstract/Overwordl/Components/NPC_COMPONENT.hpp"
#include "Abstract/Overwordl/Components/PartOfLayerComponent.hpp"
#include "Abstract/Overwordl/Components/PersistanceComponent.hpp"
#include "Abstract/Overwordl/Components/TransformComponent.hpp"
#include "Abstract/PersistenceManager/PersistenceManager.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include <cmath>
#include <spdlog/spdlog.h>
#include <string>

PersistenceRegistrationSystem::PersistenceRegistrationSystem(ArchetypeManager &manager) : System(manager) {}

void PersistenceRegistrationSystem::update()
{
	auto &persistence = PersistenceManager::getInstance();

	std::optional<EntityID> playerOpt = WorldUtils::getPlayer(manager);
	if (!playerOpt.has_value()) {
		spdlog::error("No player found in the current layer. Persistence registration skipped.");
		return;
	}
	EntityID playerId = playerOpt.value();

	// REGISTER ITEMS
	manager.view<ItemComponent, TransformComponent, PartOfLayerComponent>().each(
	    [&](EntityID id, ItemComponent &item, TransformComponent &trans, PartOfLayerComponent &layer) {
		    if (manager.hasComponent<PersistanceComponent>(id))
			    return;

		    int x = static_cast<int>(std::round(trans.position.x));
		    int y = static_cast<int>(std::round(trans.position.y));
		    std::string name = item.name.empty() ? "UNKNOWN_ITEM" : item.name;
		    std::string uuid = "ITEM_" + std::to_string(layer.groupId) + "_" + std::to_string(x) + "_"
		                       + std::to_string(y) + "_" + name;

		    manager.addComponentToEntity<PersistanceComponent>(id);
		    manager.getComponent<PersistanceComponent>(id).uuid = uuid;
	    });

	// REGISTER ENEMIES
	manager.view<CharacterComponent, TransformComponent, PartOfLayerComponent>().each(
	    [&](EntityID id, CharacterComponent &character, TransformComponent &trans, PartOfLayerComponent &layer) {
		    if (id == playerId || manager.hasComponent<PersistanceComponent>(id))
			    return;

		    int x = static_cast<int>(std::round(trans.position.x));
		    int y = static_cast<int>(std::round(trans.position.y));

		    std::string uuid =
		        "ENEMY_" + std::to_string(layer.groupId) + "_" + std::to_string(x) + "_" + std::to_string(y);

		    manager.addComponentToEntity<PersistanceComponent>(id);
		    manager.getComponent<PersistanceComponent>(id).uuid = uuid;
	    });

	manager.view<DialogComponent, InteractionComponent, TransformComponent, PartOfLayerComponent>().each(
	    [&](EntityID id, DialogComponent &dialog, InteractionComponent &interactComp, TransformComponent &trans,
	        PartOfLayerComponent &layer) {
		    if (manager.hasComponent<PersistanceComponent>(id))
			    return;

		    int x = static_cast<int>(std::round(trans.position.x));
		    int y = static_cast<int>(std::round(trans.position.y));
		    std::string uuid =
		        "Dialog_" + std::to_string(layer.groupId) + "_" + std::to_string(x) + "_" + std::to_string(y);

		    manager.addComponentToEntity<PersistanceComponent>(id);
		    manager.getComponent<PersistanceComponent>(id).uuid = uuid;
	    });

	manager.view<IsLockedComponent, TransformComponent>().each(
	    [&](EntityID id, IsLockedComponent &lockComp, TransformComponent &trans) {
		    if (manager.hasComponent<PersistanceComponent>(id))
			    return;

		    int x = static_cast<int>(std::round(trans.position.x));
		    int y = static_cast<int>(std::round(trans.position.y));
		    int keyId = lockComp.keyId;
		    std::string uuid = "LOCK_" + std::to_string(keyId) + "_" + std::to_string(x) + "_" + std::to_string(y);
		    manager.addComponentToEntity<PersistanceComponent>(id);
		    manager.getComponent<PersistanceComponent>(id).uuid = uuid;
	    });

	manager.view<PersistanceComponent>().each([&](EntityID id, PersistanceComponent &persistance) {
		spdlog::info("Entity {} registered with UUID: {}", id.getId(), persistance.uuid);
	});
}