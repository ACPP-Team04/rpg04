#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "Abstract/Utils/WorldUtlis.hpp"
#include "StatsComponent.hpp"
#include <map>

enum class WeaponType { MELEE, RANGE };

struct WeaponComponent : Component<WeaponComponent> {
	WeaponComponent() = default;
	WeaponType weaponType;
	WEAPON_SCALING_FACTOR scalingFactor;
	STATS scalingStat;
	int lightAttackBaseDmg{7};
	int heavyAttackBaseDmg{18};
	int ultimateAttackBaseDmg{35};

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
		weaponType = WorldUtils::getEnumValue<WeaponType>(j, "weapon_type");
		scalingFactor = WorldUtils::getEnumValue<WEAPON_SCALING_FACTOR>(j, "scalingFactor");
		scalingStat = WorldUtils::getEnumValue<STATS>(j, "scaleStat");
		lightAttackBaseDmg = j.get<int>("lightAttackBaseDmg");
		heavyAttackBaseDmg = j.get<int>("heavyAttackBaseDmg");
		ultimateAttackBaseDmg = j.get<int>("ultimateAttackBaseDmg");
	}

	float getScalingFactor() const { return scaleFactorAsFloat(scalingFactor); }
};
