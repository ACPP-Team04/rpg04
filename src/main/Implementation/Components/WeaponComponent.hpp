#pragma once
#include "Abstract/ECS/Component/Component.hpp"
#include "StatsComponent.hpp"
#include <map>

enum class WeaponType { MELEE, RANGE };

struct WeaponComponent : Component<WeaponComponent> {
	WeaponComponent() = default;
	WeaponType weaponType;
	WEAPON_SCALING_FACTOR scalingFactor;
	STATS scalingStat;
	int lightAttackBaseDmg{1};
	int heavyAttackBaseDmg{1};
	int ultimateAttackBaseDmg{1};

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
	void readFromJson(const nlohmann::json &j) override
	{
		weaponType = j.value("weapon_type", WeaponType());
		scalingFactor = j.value("scalingFactor", WEAPON_SCALING_FACTOR());
		scalingStat = j.value("scaleStat", STATS());
		lightAttackBaseDmg = j.value("lightAttackBaseDmg", 1);
		heavyAttackBaseDmg = j.value("heavyAttackBaseDmg", 1);
		ultimateAttackBaseDmg = j.value("ultimateAttackBaseDmg", 1);
	}

	float getScalingFactor() const { return scaleFactorAsFloat(scalingFactor); }
};
