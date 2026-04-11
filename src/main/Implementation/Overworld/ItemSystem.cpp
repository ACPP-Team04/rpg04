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

ItemSystem::ItemSystem(ArchetypeManager &manager) : System(manager) {}

void ItemSystem::update()
{
	InputComponent &inputComponent = WorldUtils::getPlayersComponent<InputComponent>(manager).value();
	InventoryComponent &inventoryComponent = WorldUtils::getPlayersComponent<InventoryComponent>(manager).value();
	WorldUtils::viewInCurrentLayer<InteractionComponent, ItemComponent>(manager,
	    [&](auto &entity, InteractionComponent &interaction, ItemComponent &item) {
		    if (!interaction.isActive) {
			    return;
		    }
		    if (interaction.action == INTERACTION_ACTION::PICK_ITEM) {
		    	if (inputComponent.interact.justPressed) {
		    		return;
		    	}
		    	inventoryComponent.addItem(entity, item.itemType);
		    	this->manager.removeComponentFromEntity<RenderComponent>(entity);
		    	this->manager.removeComponentFromEntity<InteractionComponent>(entity);
		    }
	    });
}