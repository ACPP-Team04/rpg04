#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
struct START_EQUIPMENT_COMPONENT : Component<START_EQUIPMENT_COMPONENT> {
	int healing;
	tson::TiledClass  rawWeaponComponent;

	void readFromJson(tson::TiledClass &j) override

	{
		healing = j.get<int>("healing");
		rawWeaponComponent = j.get<tson::TiledClass>("weapon");

	}
};