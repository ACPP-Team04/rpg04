#pragma once
#include "Abstract/Overwordl/ItemSystem.hpp"

#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"

ItemSystem::ItemSystem(ArchetypeManager &manager) : System(manager) {}

void ItemSystem::update()
{
	std::vector<EntityID> entities;

	this->manager.view<PlayerComponent, InventoryComponent>().each(
	    [&](auto &entity, PlayerComponent &player, InventoryComponent &inventory) { entities.push_back(entity); });
	this->manager.view<InteractionComponent, ItemComponent>().each(
	    [&](auto &entity, InteractionComponent &interaction, ItemComponent &item) {
		    if (!interaction.isActive) {
			    return;
		    }
		    if (interaction.action == INTERACTION_ACTION::PICK_ITEM) {
			    for (auto player : entities) {
				    if (!this->manager.getComponent<InputComponent>(player).interact.justPressed) {
					    continue;
				    }
				    this->manager.getComponent<InventoryComponent>(player).inventory.push_back(entity);
				    this->manager.removeComponentFromEntity<RenderComponent>(entity);
				    this->manager.removeComponentFromEntity<InteractionComponent>(entity);
			    }
		    }
	    });
}