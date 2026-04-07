#include "Abstract/Overwordl/DoorSystem.hpp"

#include "Abstract/Overwordl/Components.hpp"

#include <mutex>

DoorSystem::DoorSystem(ArchetypeManager &manager) : System(manager) {}

bool inventoryHasKey(ArchetypeManager &manager, InventoryComponent &comp, ITEM_KEYS key)
{
	for (auto &item : comp.inventory) {
		if (!manager.hasComponent<ItemKeyComponent>(item))
			continue;
		if (manager.getComponent<ItemKeyComponent>(item).key == key)
			return true;
	}
	return false;
}
void DoorSystem::update()
{
	InventoryComponent *comp;
	bool playerFound = false;
	this->manager.view<PlayerComponent, InventoryComponent>().each([&](auto &entity, auto &component, auto &inventory) {
		comp = &inventory;
		playerFound = true;
	});

	if (!playerFound) {
		return;
	}
	this->manager.view<InteractionComponent, IsLockedComponent>().each(
	[&](EntityID id, InteractionComponent &icomp, IsLockedComponent &lcomp)  {
		    if (!icomp.isActive) {
			    return;
		    }

		    if (icomp.action == DOOR_INTERACTION) {
			    if (lcomp.isLocked) {
				    if (inventoryHasKey(manager, *comp, lcomp.key)) {
					    lcomp.isLocked = false;
					    icomp.action = INTERACTION_ACTION::SWITCH_LAYER;
				    } else {
					    std::cout << "Door is closed!!!" << std::endl;
				    }
			    }
		    	else {
		    		std::cout << "Door open!!!" << std::endl;
		    	}
		    }
	    });
}