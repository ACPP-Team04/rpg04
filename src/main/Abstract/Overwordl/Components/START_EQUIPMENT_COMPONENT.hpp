#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/TILE_ENUMS.hpp"
#include "Implementation/Components/WeaponComponent.hpp"
struct START_EQUIPMENT_COMPONENT : Component<START_EQUIPMENT_COMPONENT> {
	int healing;
	nlohmann::json rawWeaponComponent;

	void readFromJson(const nlohmann::json &j) override
	{
		healing = j.value("healing", 0);
		rawWeaponComponent = j.value("weapon",nlohmann::json());

	}
};