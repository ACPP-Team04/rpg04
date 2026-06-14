#include "Abstract/Overwordl/DoorSystem.hpp"

#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/IsLockedComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"

#include <mutex>
#include <spdlog/spdlog.h>

DoorSystem::DoorSystem(ArchetypeManager &manager) : System(manager) {}

bool inventoryHasKey(ArchetypeManager &manager, CharacterComponent &comp, int keyItem)
{
	return manager.getComponent<PartOfLayerComponent>(keyItem).groupId == comp.inventory.inventoryWorldId;
}
void DoorSystem::update()
{
	CharacterComponent &comp = WorldUtils::getPlayersComponent<CharacterComponent>(manager).value();
	WorldUtils::viewInCurrentLayer<InteractionComponent, IsLockedComponent>(
	    manager, [&](EntityID id, InteractionComponent &icomp, IsLockedComponent &lcomp) {
		    if (!icomp.isActive) {
			    return;
		    }
		    if (icomp.action == DOOR_INTERACTION) {
			    if (lcomp.isLocked) {
				    if (inventoryHasKey(manager, comp, lcomp.keyId)) {
					    lcomp.isLocked = false;
					    icomp.action = INTERACTION_ACTION::SWITCH_LAYER;
				    } else {
					    spdlog::debug("Door is closed!!");
				    }
			    } else {
				    icomp.action = SWITCH_LAYER;
				    spdlog::debug("Door open!!");
			    }
		    }
	    });
}