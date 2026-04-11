#include "Abstract/Overwordl/DoorSystem.hpp"

#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/IsLockedComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

#include <mutex>

DoorSystem::DoorSystem(ArchetypeManager &manager) : System(manager) {}

bool inventoryHasKey(ArchetypeManager &manager, InventoryComponent &comp, int keyItem)
{
	return comp.containsItem(keyItem);
}
void DoorSystem::update()
{
	InventoryComponent &comp = WorldUtils::getPlayersComponent<InventoryComponent>(manager).value();
	WorldUtils::viewInCurrentLayer<InteractionComponent, IsLockedComponent>(manager,
	    [&](EntityID id, InteractionComponent &icomp, IsLockedComponent &lcomp) {
		    if (!icomp.isActive) {
			    return;
		    }
		    if (icomp.action == DOOR_INTERACTION) {
			    if (lcomp.isLocked) {
				    if (inventoryHasKey(manager, comp, lcomp.keyId)) {
					    lcomp.isLocked = false;
					    icomp.action = INTERACTION_ACTION::SWITCH_LAYER;
				    } else {
					    std::cout << "Door is closed!!!" << std::endl;
				    }
			    } else {
				    std::cout << "Door open!!!" << std::endl;
			    }
		    }
	    });
}