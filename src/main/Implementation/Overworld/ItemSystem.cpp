#pragma once
#include "Abstract/Overwordl/ItemSystem.hpp"

#include "Abstract/Overwordl/Components.hpp"

ItemSystem::ItemSystem(ArchetypeManager &manager) : System(manager) {}

void ItemSystem::update()
{
	std::vector<EntityID> entities;

	this->manager.view<PlayerComponent, InventoryComponent>().each(
	    [&](auto entity, PlayerComponent &player, InventoryComponent &inventory) { entities.push_back(entity); });
	this->manager.view<InteractionComponent, ItemHealingComponent>().each(
	    [&](auto entity, InteractionComponent &interaction, ItemHealingComponent &item) {
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
	this->manager.view<InteractionComponent, ItemKeyComponent>().each(
		[&](auto entity, InteractionComponent &interaction, ItemKeyComponent &item) {
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