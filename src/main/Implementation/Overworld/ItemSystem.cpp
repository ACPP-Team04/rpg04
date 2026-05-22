#pragma once
#include "Abstract/Overwordl/ItemSystem.hpp"

#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/WorldParser.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include <Abstract/Overwordl/Components/BoundingBoxComponent.hpp>
#include <Abstract/Overwordl/Components/CollisionComponent.hpp>
#include <spdlog/spdlog.h>

ItemSystem::ItemSystem(ArchetypeManager &manager) : System(manager) {}

void ItemSystem::update()
{
	auto inputComponentOptional = WorldUtils::getPlayersComponent<InputComponent>(manager);
	if (!inputComponentOptional.has_value()) {
		return;
	}
	InputComponent &inputComponent = inputComponentOptional.value();
	InventoryComponent &inventoryComponent = WorldUtils::getPlayersComponent<InventoryComponent>(manager).value();

	std::vector<EntityID> itemsToPickup;
	WorldUtils::viewInCurrentLayer<InteractionComponent, ItemComponent>(
	    manager, [&](auto &entity, InteractionComponent &interaction, ItemComponent &item) {
		    if (!interaction.isActive) {
			    return;
		    }
		    if (interaction.action == INTERACTION_ACTION::PICK_ITEM) {
			    if (!inputComponent.interact.justPressed) {
				    return;
			    }
			    inventoryComponent.addItem(entity, item.itemType);
			    itemsToPickup.push_back(entity);
			    spdlog::info("Picked up item: {}", item.name);
		    }
	    });
	for (EntityID entity : itemsToPickup) {
		manager.removeComponentFromEntity<RenderComponent>(entity);
		manager.removeComponentFromEntity<InteractionComponent>(entity);

		if (manager.hasComponent<CollisionComponent>(entity)) {
			manager.removeComponentFromEntity<CollisionComponent>(entity);
		}
		if (manager.hasComponent<BoundIngBoxComponent>(entity)) {
			manager.removeComponentFromEntity<BoundIngBoxComponent>(entity);
		}
	}
}
