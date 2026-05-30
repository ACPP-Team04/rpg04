#include "Abstract/Overwordl/ItemSystem.hpp"

#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/InputComponent.hpp"
#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/InventoryComponent.hpp"
#include "Abstract/Overwordl/Components/IsInInventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemComponent.hpp"
#include "Abstract/Overwordl/Components/PersistanceComponent.hpp"
#include "Abstract/Overwordl/Components/Player_Component.hpp"
#include "Abstract/Overwordl/Components/RenderComponent.hpp"
#include "Abstract/Overwordl/WorldParser.hpp"
#include "Abstract/PersistenceManager/PersistenceManager.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include <Abstract/Overwordl/Components/CollisionComponent.hpp>
#include <spdlog/spdlog.h>

ItemSystem::ItemSystem(ArchetypeManager& manager) : System(manager) {}

void ItemSystem::update()
{
	WorldComponent* world = WorldUtils::getWorld(manager);
	EntityID player = WorldUtils::getPlayer(manager).value();

	if (!manager.hasComponent<InputComponent>(player)) {
		return;
	}
	CharacterComponent& character_component = manager.getComponent<CharacterComponent>(player);
	std::cout << "CharacterCatch" << std::endl;
	InputComponent& inputComponent = manager.getComponent<InputComponent>(player);
	std::cout << character_component.inventory.items.size() << std::endl;
	WorldUtils::viewInCurrentLayer<InteractionComponent, ItemComponent>(
		manager, [&](auto& entity, InteractionComponent& interaction, ItemComponent& item) {
			if (!interaction.isActive) {
				return;
			}
			if (interaction.action == INTERACTION_ACTION::PICK_ITEM) {
				if (inputComponent.interact.justPressed) {
					return;
				}

				world->pushMessageToHud("New Item Added");
				manager.getComponent<PartOfLayerComponent>(entity).groupId =
					character_component.inventory.inventoryWorldId;
				if (manager.hasComponent<PersistanceComponent>(entity)) {
					std::string uuid = manager.getComponent<PersistanceComponent>(entity).uuid;
					GameState::getInstance().deadUniqueEntities.insert(uuid);
					spdlog::info("Item picked up! UUID {} added to dead entities.", uuid);
				}
			}
		});
}