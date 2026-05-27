#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
struct ItemComponent : public Component<ItemComponent> {
	ITEM_TYPE itemType;
	std::string name;
	WeaponComponent weaponStats;

	static float scaleFactorAsFloat(WEAPON_SCALING_FACTOR factor)
	{
		switch (factor) {
		case SCALE_A:
			return 1.0;
		case SCALE_B:
			return 0.5;
		case SCALE_C:
			return 0.25;
		default:
			return 0.0;
		}
	}

	void readFromJson(tson::TiledClass &j) override
	{
		itemType = WorldUtils::getEnumValue<ITEM_TYPE>(j,"itemType");
		name = j.get<std::string>("name");
		if (itemType != WEAPON) return;
		auto weaponstats = j.get<tson::TiledClass>("weaponStats");
		weaponStats.readFromJson(weaponstats);


	}
};
