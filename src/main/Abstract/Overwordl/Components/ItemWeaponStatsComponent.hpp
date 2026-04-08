#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"

struct ITEM_WEAPON_STATS_KOMPONENT : public Component<ITEM_WEAPON_STATS_KOMPONENT> {
	std::string name;
	ITEM_WEAPONS_TYPE type;
	void readFromJson(const nlohmann::json &j) override
	{
		type = j.value("type", ITEM_WEAPONS_TYPE());
		name = j.value("name", name);
	}
};