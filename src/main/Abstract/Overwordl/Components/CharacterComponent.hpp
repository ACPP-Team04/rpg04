#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Implementation/Components/StatsComponent.hpp"
#include "InventoryComponent.hpp"

struct CharacterComponent : Component<CharacterComponent> {

	bool fightable;
	InventoryComponent inventory;
	StatsComponent stats;
	int equipedWeapon;
	int equipedCompanion;
	void readFromJson(tson::TiledClass &j) override
	{
		fightable = j.get<bool>("fightable");
		auto inv = j.get<tson::TiledClass>("inventory");
		inventory.readFromJson(inv);
		equipedWeapon = j.get<unsigned int>("equippedWeaponId");
		equipedCompanion = j.get<unsigned int>("equippedCompanion");
		auto stat = j.get<tson::TiledClass>("stats");
		stats.readFromJson(stat);
	}
};