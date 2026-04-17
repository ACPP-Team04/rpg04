#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
struct ItemComponent : public Component<ItemComponent> {
	ITEM_TYPE itemType;
	bool equiped = false;

	void readFromJson(tson::TiledClass &j) override
	{
		itemType = WorldUtils::getEnumValue<ITEM_TYPE>(j,"itemType");
	}
};
