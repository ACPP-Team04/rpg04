#include "Abstract/Overwordl/CharacterPreProcessSystem.hpp"

#include "Abstract/Overwordl/Components/CharacterComponent.hpp"
#include "Abstract/Overwordl/Components/ItemComponent.hpp"
#include <Abstract/Overwordl/Components/PersistanceComponent.hpp>

CharacterPreProcessSystem::CharacterPreProcessSystem(ArchetypeManager &manger) : System(manger) {}

void CharacterPreProcessSystem::update()
{
	std::vector<std::pair<int, EntityID>> inventoryWorlds;

	this->manager.view<CharacterComponent>().each([&](auto &entity, CharacterComponent &component) {
		if (!component.fightable)
			return;
		inventoryWorlds.push_back({component.inventory.inventoryWorldId, entity});
	});

	for (auto &[worldId, charEntity] : inventoryWorlds) {
		bool hasWeapon = false;
		this->manager.view<ItemComponent, PartOfLayerComponent>().each(
		    [&](auto &item, ItemComponent &itemComp, PartOfLayerComponent &layer) {
			    if (layer.groupId == worldId && itemComp.itemType == ITEM_TYPE::WEAPON) {
				    hasWeapon = true;
			    }
		    });

		if (!hasWeapon) {
			EntityID fist = manager.createEntity<ItemComponent, PartOfLayerComponent, PersistanceComponent>();
			manager.getComponent<ItemComponent>(fist).itemType = ITEM_TYPE::WEAPON;
			manager.getComponent<ItemComponent>(fist).name = "FIST";
			manager.getComponent<PartOfLayerComponent>(fist).groupId = worldId;
			manager.getComponent<CharacterComponent>(charEntity).equipedWeapon = fist.getId();
			manager.getComponent<PersistanceComponent>(fist).uuid =
			    "DEFAULT_FIST_" + std::to_string(charEntity.getId());
			spdlog::info("Added fists to character {}", charEntity.getId());
		} else {
			this->manager.view<ItemComponent, PartOfLayerComponent>().each(
			    [&](auto &item, ItemComponent &itemComp, PartOfLayerComponent &layer) {
				    if (layer.groupId == worldId && itemComp.itemType == ITEM_TYPE::WEAPON) {
					    manager.getComponent<CharacterComponent>(charEntity).equipedWeapon = item.getId();
				    }
			    });
		}
	}
}