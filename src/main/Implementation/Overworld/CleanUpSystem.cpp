
#include "Abstract/Overwordl/CleanUpSystem.hpp"

#include "Abstract/Overwordl/Components/InteractionComponent.hpp"
#include "Abstract/Overwordl/Components/IsInInventoryComponent.hpp"
#include "Abstract/Overwordl/Components/ItemComponent.hpp"
#include "Abstract/Overwordl/Components/NPC_COMPONENT.hpp"
#include "Abstract/Overwordl/Components/SpriteComponent.hpp"
CleanUpSystem::CleanUpSystem(ArchetypeManager &manager) : System(manager) {}

void CleanUpSystem::update()
{
	std::vector<EntityID> entities;
	manager.view<IsInInventoryComponent>().each([&](auto &entity, auto &component) { entities.push_back(entity); });

	for (auto &entity : entities) {
		manager.removeAllExcept<ItemComponent, IsInInventoryComponent, SpriteComponent, NPC_Component>(entity);
	}
}