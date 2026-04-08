#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/ECS/Entity/EntityID.hpp"
#include "Abstract/TILE_ENUMS.hpp"

struct InventoryComponent : public Component<InventoryComponent> {
	std::vector<EntityID> inventory;
	std::unordered_map<ITEM_TYPE, std::unordered_set<EntityID>> items;

	void readFromJson(const nlohmann::json &j) override {}

	void addItem(EntityID entity, ITEM_TYPE item)
	{
		if (!items.contains(item)) {
			items.insert({item, {}});
		}

		items[item].insert(entity);
	}

	void removeItem(EntityID entity, ITEM_TYPE item)
	{
		if (items.contains(item)) {
			items[item].erase(entity);
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
};