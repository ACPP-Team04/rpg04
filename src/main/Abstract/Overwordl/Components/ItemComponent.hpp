#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
struct ItemComponent : public Component<ItemComponent> {
	ITEM_TYPE itemType;
	bool equiped = false;

	void readFromJson(const nlohmann::json &j) override { itemType = j.value("itemType", ITEM_TYPE()); }
};
