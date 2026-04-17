#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/TILE_ENUMS.hpp"

struct InventoryComponent : public Component<InventoryComponent> {
	std::vector<EntityID> inventory;
	std::unordered_map<ITEM_TYPE, std::unordered_set<EntityID>> items;

	std::unordered_map<ITEM_TYPE, EntityID> equiped;

	void readFromJson(tson::TiledClass &j) override
	{

	}

	void addItem(EntityID entity, ITEM_TYPE item)
	{
		if (!items.contains(item)) {
			items.insert({item, {}});
		}

		items[item].insert(entity);
		if (!equiped.contains(item)) {
			equiped[item] = entity;
		}
	}

	void removeItem(EntityID entity, ITEM_TYPE item)
	{
		if (items.contains(item)) {
			items[item].erase(entity);

			if (getEquippedItem(item) == entity) {
				equiped.erase(item);
			}
		}
	}

	std::unordered_set<EntityID> getItems(ITEM_TYPE item)
	{
		if (items.contains(item)) {
			return items[item];
		}

		return {};
	}

	bool containsItem(int entity)
	{
		for (auto &item : items) {
			if (item.second.contains(entity)) {
				return true;
			}
		}
		return false;
	}

	bool containsItem(EntityID entity) const
	{
		for (auto &item : items) {
			if (item.second.contains(entity)) {
				return true;
			}
		}
		return false;
	}

	void equip(EntityID itemId, ITEM_TYPE item)
	{
		if (containsItem(itemId)) {
			this->equiped[item] = itemId;
		} else {
			throw std::runtime_error("Inventory does not have this item to equip");
		}
	}

	EntityID getEquippedItem(ITEM_TYPE item)
	{
		if (items.contains(item)) {
			if (equiped.contains(item)) {
				return equiped[item];
			}
		} 
		throw std::runtime_error("No equipment");
		
	}

	bool hasEquippedItem(ITEM_TYPE item)
	{
		if (items.contains(item)) {
			if (equiped.contains(item)) {
				return true;
			}
		} else {
			return false;
		}
		return false;
	}
};
