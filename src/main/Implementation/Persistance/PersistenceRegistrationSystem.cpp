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

	std::vector<std::pair<EntityID, std::string>> commandBuffer;

	// REGISTER ITEMS
	manager.view<ItemComponent, TransformComponent, PartOfLayerComponent>().each(
	    [this, &commandBuffer](EntityID id, ItemComponent &item, TransformComponent &trans,
	                           PartOfLayerComponent &layer) {
		    if (manager.hasComponent<PersistanceComponent>(id))
			    return;

		    auto x = static_cast<int>(std::round(trans.position.x));
		    auto y = static_cast<int>(std::round(trans.position.y));
		    std::string name = item.name.empty() ? "UNKNOWN_ITEM" : item.name;
		    std::string uuid = std::format("ITEM_{}_{}_{}_{}", layer.groupId, x, y, name);
		    commandBuffer.emplace_back(id, uuid);
	    });

	for (const auto &[id, uuid] : commandBuffer) {
		manager.addComponentToEntity<PersistanceComponent>(id);
		manager.getComponent<PersistanceComponent>(id).uuid = uuid;
	}
	commandBuffer.clear();

	// REGISTER ENEMIES
	manager.view<CharacterComponent, TransformComponent, PartOfLayerComponent>().each(
	    [this, &commandBuffer, &playerId](EntityID id, [[maybe_unused]] CharacterComponent &character,
	                                      const TransformComponent &trans, PartOfLayerComponent &layer) {
		    if (id == playerId || manager.hasComponent<PersistanceComponent>(id))
			    return;

		    auto x = static_cast<int>(std::round(trans.position.x));
		    auto y = static_cast<int>(std::round(trans.position.y));

		    std::string uuid = std::format("ENEMY_{}_{}_{}", layer.groupId, x, y);
		    commandBuffer.emplace_back(id, uuid);
	    });

	for (const auto &[id, uuid] : commandBuffer) {
		manager.addComponentToEntity<PersistanceComponent>(id);
		manager.getComponent<PersistanceComponent>(id).uuid = uuid;
	}
	commandBuffer.clear();

	manager.view<DialogComponent, InteractionComponent, TransformComponent, PartOfLayerComponent>().each(
	    [this, &commandBuffer](EntityID id, DialogComponent &dialog,
	                           [[maybe_unused]] InteractionComponent &interactComp, const TransformComponent &trans,
	                           PartOfLayerComponent &layer) {
		    if (manager.hasComponent<PersistanceComponent>(id))
			    return;

		    auto x = static_cast<int>(std::round(trans.position.x));
		    auto y = static_cast<int>(std::round(trans.position.y));
		    std::string uuid = std::format("Dialog_{}_{}_{}_{}_{})", layer.groupId, x, y, dialog.currentNodeIndex,
		                                   dialog.nodes.size());
		    commandBuffer.emplace_back(id, uuid);
	    });
	for (const auto &[id, uuid] : commandBuffer) {
		manager.addComponentToEntity<PersistanceComponent>(id);
		manager.getComponent<PersistanceComponent>(id).uuid = uuid;
	}
	commandBuffer.clear();

	manager.view<IsLockedComponent, TransformComponent>().each(
	    [this, &commandBuffer](EntityID id, const IsLockedComponent &lockComp, const TransformComponent &trans) {
		    if (manager.hasComponent<PersistanceComponent>(id))
			    return;

		    auto x = static_cast<int>(std::round(trans.position.x));
		    auto y = static_cast<int>(std::round(trans.position.y));
		    int keyId = lockComp.keyId;
		    std::string uuid = std::format("LOCK_{}_{}_{}", keyId, x, y);
		    commandBuffer.emplace_back(id, uuid);
	    });

	for (const auto &[id, uuid] : commandBuffer) {
		manager.addComponentToEntity<PersistanceComponent>(id);
		manager.getComponent<PersistanceComponent>(id).uuid = uuid;
	}
	commandBuffer.clear();
	manager.view<PersistanceComponent>().each([&](EntityID id, PersistanceComponent &persistance) {
		spdlog::info("Entity {} registered with UUID: {}", id.getId(), persistance.uuid);
	});
}